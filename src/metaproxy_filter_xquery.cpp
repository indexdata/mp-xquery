/* This file is part of mp-xquery
   Copyright (C) Index Data

Metaproxy is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

Metaproxy is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <metaproxy/package.hpp>
#include <metaproxy/util.hpp>
#include <yaz/log.h>
#include <yaz/oid_db.h>
#include <yaz/diagbib1.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <zorba/zorba.h>
#include <zorba/store_manager.h>
#include <zorba/serializer.h>
#include <zorba/singleton_item_sequence.h>
#include <zorba/zorba_exception.h>


namespace mp = metaproxy_1;
namespace yf = mp::filter;
namespace mp_util = metaproxy_1::util;
using namespace mp;
using namespace zorba;

namespace metaproxy_1 {
    namespace filter {
        class XQuery : public Base {
        public:
            ~XQuery();
            XQuery();
            void process(metaproxy_1::Package & package) const;
            void configure(const xmlNode * ptr, bool test_only,
                           const char *path);
            void start() const;
            void stop(int signo) const;
        private:
            bool convert_one_record(const char *input_buf,
                                    size_t input_len,
                                    std::string &result) const;
            std::map<std::string, std::string> zorba_variables;
            std::string zorba_filename;
            std::string zorba_script;
            std::string zorba_record_variable;
            Zorba *lZorba;
            XQuery_t lQuery;
        };
    }
}

yf::XQuery::XQuery()
{
    lZorba = 0;
}

yf::XQuery::~XQuery()
{
    if (lZorba)
        lZorba->shutdown();
}

void yf::XQuery::start() const
{
}

void yf::XQuery::stop(int signo) const
{
}

bool yf::XQuery::convert_one_record(const char *input_buf,
                                    size_t input_len,
                                    std::string &result) const
{
    XQuery_t tQuery = lQuery->clone();

    zorba::DynamicContext* lDynamicContext = tQuery->getDynamicContext();

    zorba::Item lItem;
    std::map<std::string, std::string>::const_iterator it;
    for (it = zorba_variables.begin(); it != zorba_variables.end(); it++)
    {
        lItem = lZorba->getItemFactory()->createString(it->second);
        lDynamicContext->setVariable(it->first, lItem);
    }
    std::string rec_content = "raw:" + std::string(input_buf, input_len);
    lItem = lZorba->getItemFactory()->createString(rec_content);
    lDynamicContext->setVariable(zorba_record_variable, lItem);

    try {
        std::stringstream ss;
        tQuery->execute(ss);
        result = ss.str();
        return true;
    } catch ( ZorbaException &e) {
        result = e.what();
        yaz_log(YLOG_WARN, "XQuery execute: %s", result.c_str());
        return false;
    }
}

void yf::XQuery::process(Package &package) const
{
    Z_GDU *gdu_req = package.request().get();
    Z_PresentRequest *pr_req = 0;
    Z_SearchRequest *sr_req = 0;

    const char *input_schema = 0;
    Odr_oid *input_syntax = 0;

    if (gdu_req && gdu_req->which == Z_GDU_Z3950 &&
        gdu_req->u.z3950->which == Z_APDU_presentRequest)
    {
        pr_req = gdu_req->u.z3950->u.presentRequest;

        input_schema =
            mp_util::record_composition_to_esn(pr_req->recordComposition);
        input_syntax = pr_req->preferredRecordSyntax;
    }
    else if (gdu_req && gdu_req->which == Z_GDU_Z3950 &&
             gdu_req->u.z3950->which == Z_APDU_searchRequest)
    {
        sr_req = gdu_req->u.z3950->u.searchRequest;

        input_syntax = sr_req->preferredRecordSyntax;

        // we don't know how many hits we're going to get and therefore
        // the effective element set name.. Therefore we can only allow
        // two cases.. Both equal or absent.. If not, we'll just have to
        // disable the piggyback!
        if (sr_req->smallSetElementSetNames
            &&
            sr_req->mediumSetElementSetNames
            &&
            sr_req->smallSetElementSetNames->which == Z_ElementSetNames_generic
            &&
            sr_req->mediumSetElementSetNames->which == Z_ElementSetNames_generic
            &&
            !strcmp(sr_req->smallSetElementSetNames->u.generic,
                    sr_req->mediumSetElementSetNames->u.generic))
        {
            input_schema = sr_req->smallSetElementSetNames->u.generic;
        }
        else if (!sr_req->smallSetElementSetNames &&
                 !sr_req->mediumSetElementSetNames)
            ; // input_schema is 0 already
        else
        {
            // disable piggyback (perhaps it was disabled already)
            *sr_req->smallSetUpperBound = 0;
            *sr_req->largeSetLowerBound = 0;
            *sr_req->mediumSetPresentNumber = 0;
            package.move();
            return;
        }
        // we can handle it in record_transform.
    }
    else
    {
        package.move();
        return;
    }

    mp::odr odr_en(ODR_ENCODE);

    const char *backend_schema = 0;
    const Odr_oid *backend_syntax = 0;

    if (input_schema && !strcmp(input_schema, "bibframe") &&
        (!input_syntax || !oid_oidcmp(input_syntax, yaz_oid_recsyn_xml)))
    {
        backend_schema = "marcxml";
        backend_syntax = yaz_oid_recsyn_xml;
    }
    else
    {
        package.move();
        return;
    }

    if (sr_req)
    {
        if (backend_syntax)
            sr_req->preferredRecordSyntax = odr_oiddup(odr_en, backend_syntax);
        else
            sr_req->preferredRecordSyntax = 0;
        if (backend_schema)
        {
            sr_req->smallSetElementSetNames
                = (Z_ElementSetNames *)
                odr_malloc(odr_en, sizeof(Z_ElementSetNames));
            sr_req->smallSetElementSetNames->which = Z_ElementSetNames_generic;
            sr_req->smallSetElementSetNames->u.generic
                = odr_strdup(odr_en, backend_schema);
            sr_req->mediumSetElementSetNames = sr_req->smallSetElementSetNames;
        }
        else
        {
            sr_req->smallSetElementSetNames = 0;
            sr_req->mediumSetElementSetNames = 0;
        }
    }
    else if (pr_req)
    {
        if (backend_syntax)
            pr_req->preferredRecordSyntax = odr_oiddup(odr_en, backend_syntax);
        else
            pr_req->preferredRecordSyntax = 0;

        if (backend_schema)
        {
            pr_req->recordComposition
                = (Z_RecordComposition *)
                odr_malloc(odr_en, sizeof(Z_RecordComposition));
            pr_req->recordComposition->which
                = Z_RecordComp_simple;
            pr_req->recordComposition->u.simple
                = (Z_ElementSetNames *)
                odr_malloc(odr_en, sizeof(Z_ElementSetNames));
            pr_req->recordComposition->u.simple->which = Z_ElementSetNames_generic;
            pr_req->recordComposition->u.simple->u.generic
                = odr_strdup(odr_en, backend_schema);
        }
        else
            pr_req->recordComposition = 0;
    }
    package.move();

    Z_GDU *gdu_res = package.response().get();

    // see if we have a records list to patch!
    Z_NamePlusRecordList *records = 0;
    if (gdu_res && gdu_res->which == Z_GDU_Z3950 &&
        gdu_res->u.z3950->which == Z_APDU_presentResponse)
    {
        Z_PresentResponse * pr_res = gdu_res->u.z3950->u.presentResponse;

        if (pr_res
            && pr_res->numberOfRecordsReturned
            && *(pr_res->numberOfRecordsReturned) > 0
            && pr_res->records
            && pr_res->records->which == Z_Records_DBOSD)
        {
            records = pr_res->records->u.databaseOrSurDiagnostics;
        }
    }
    if (gdu_res && gdu_res->which == Z_GDU_Z3950 &&
        gdu_res->u.z3950->which == Z_APDU_searchResponse)
    {
        Z_SearchResponse *sr_res = gdu_res->u.z3950->u.searchResponse;

        if (sr_res
            && sr_res->numberOfRecordsReturned
            && *(sr_res->numberOfRecordsReturned) > 0
            && sr_res->records
            && sr_res->records->which == Z_Records_DBOSD)
        {
            records = sr_res->records->u.databaseOrSurDiagnostics;
        }
    }
    if (records)
    {
        int i;
        for (i = 0; i < records->num_records; i++)
        {
            Z_NamePlusRecord **npr = &records->records[i];
            if ((*npr)->which == Z_NamePlusRecord_databaseRecord)
            {
                const char *details = 0;
                Z_External *r = (*npr)->u.databaseRecord;
                int ret_trans = -1;
                if (r->which == Z_External_octet &&
                    !oid_oidcmp(r->direct_reference, yaz_oid_recsyn_xml))
                {
                    std::string result;
                    if (convert_one_record(
                        r->u.octet_aligned->buf, r->u.octet_aligned->len,
                        result))
                    {
                        (*npr)->u.databaseRecord =
                            z_ext_record_oid(odr_en, yaz_oid_recsyn_xml,
                                             result.c_str(),
                                             result.length());
                    }
                    else
                    {
                        *npr = zget_surrogateDiagRec(
                            odr_en, (*npr)->databaseName,
                            YAZ_BIB1_SYSTEM_ERROR_IN_PRESENTING_RECORDS,
                            result.c_str());
                    }
                }
            }
        }
        package.response() = gdu_res;
    }
}

void yf::XQuery::configure(const xmlNode * ptr, bool test_only,
                           const char *path)
{
    for (ptr = ptr->children; ptr; ptr = ptr->next)
    {
        if (ptr->type != XML_ELEMENT_NODE)
            continue;
        if (!strcmp((const char *) ptr->name, "variable"))
        {
            std::string name;
            std::string value;
            struct _xmlAttr *attr;
            for (attr = ptr->properties; attr; attr = attr->next)
                if (!strcmp((const char *) attr->name, "name"))
                    name = mp::xml::get_text(attr->children);
                else if (!strcmp((const char *) attr->name, "value"))
                    value = mp::xml::get_text(attr->children);
                else
                    throw mp::filter::FilterException(
                        "Bad attribute " + std::string((const char *)
                                                       attr->name));
            if (name.length() > 0)
                zorba_variables[name] = value;
        }
        else if (!strcmp((const char *) ptr->name, "script"))
        {
            std::string name;
            struct _xmlAttr *attr;
            for (attr = ptr->properties; attr; attr = attr->next)
                if (!strcmp((const char *) attr->name, "name"))
                    name = mp::xml::get_text(attr->children);
                else
                    throw mp::filter::FilterException(
                        "Bad attribute " + std::string((const char *)
                                                       attr->name));
            zorba_script = name;
        }
        else if (!strcmp((const char *) ptr->name, "record"))
        {
            std::string name;
            struct _xmlAttr *attr;
            for (attr = ptr->properties; attr; attr = attr->next)
                if (!strcmp((const char *) attr->name, "name"))
                    name = mp::xml::get_text(attr->children);
                else
                    throw mp::filter::FilterException(
                        "Bad attribute " + std::string((const char *)
                                                       attr->name));
            zorba_record_variable = name;
        }
        else
        {
            throw mp::filter::FilterException("Bad element "
                                               + std::string((const char *)
                                                             ptr->name));
        }
    }
    if (zorba_script.length() == 0)
        throw mp::filter::FilterException("Missing element script");
    if (zorba_record_variable.length() == 0)
        throw mp::filter::FilterException("Missing element record");
    if (!test_only)
    {
        void* lStore = StoreManager::getStore();
        lZorba = Zorba::getInstance(lStore);

        lQuery = lZorba->createQuery();

        try {
            size_t t = zorba_script.find_last_of('/');
            if (t != std::string::npos)
                lQuery->setFileName(zorba_script.substr(0, t + 1));
            std::unique_ptr<std::istream> qfile(
                new std::ifstream(zorba_script.c_str()));
            Zorba_CompilerHints lHints;
            lQuery->compile(*qfile, lHints);
        } catch ( ZorbaException &e) {
            std::string msg = "XQuery compile: ";
            msg += e.what();
            throw mp::filter::FilterException(msg);
        }
    }
}

static yf::Base* filter_creator()
{
    return new mp::filter::XQuery;
}

extern "C" {
    struct metaproxy_1_filter_struct metaproxy_1_filter_xquery = {
        0,
        "xquery",
        filter_creator
    };
}


/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

