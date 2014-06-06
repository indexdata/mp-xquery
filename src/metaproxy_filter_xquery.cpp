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
#include <map>

namespace mp = metaproxy_1;
namespace yf = mp::filter;
namespace mp_util = metaproxy_1::util;
using namespace mp;

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
            std::map<std::string, std::string> zorba_variables;
            std::string zorba_filename;
            std::string zorba_script;
            std::string zorba_record_variable;
        };
    }
}

yf::XQuery::XQuery()
{
}

yf::XQuery::~XQuery()
{

}


void yf::XQuery::start() const
{
}

void yf::XQuery::stop(int signo) const
{
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
    package.move();
}

void yf::XQuery::configure(const xmlNode * ptr, bool test_only,
                           const char *path)
{
    for (ptr = ptr->children; ptr; ptr = ptr->next)
    {
        if (ptr->type != XML_ELEMENT_NODE)
            continue;
        if (!strcmp((const char *) ptr->name, "setVariable"))
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
        else if (!strcmp((const char *) ptr->name, "filename"))
        {
            std::string value;
            struct _xmlAttr *attr;
            for (attr = ptr->properties; attr; attr = attr->next)
                if (!strcmp((const char *) attr->name, "value"))
                    value = mp::xml::get_text(attr->children);
                else
                    throw mp::filter::FilterException(
                        "Bad attribute " + std::string((const char *)
                                                       attr->name));
            zorba_filename = value;
        }
        else if (!strcmp((const char *) ptr->name, "script"))
        {
            std::string value;
            struct _xmlAttr *attr;
            for (attr = ptr->properties; attr; attr = attr->next)
                if (!strcmp((const char *) attr->name, "value"))
                    value = mp::xml::get_text(attr->children);
                else
                    throw mp::filter::FilterException(
                        "Bad attribute " + std::string((const char *)
                                                       attr->name));
            zorba_script = value;
        }
        else if (!strcmp((const char *) ptr->name, "record"))
        {
            std::string value;
            struct _xmlAttr *attr;
            for (attr = ptr->properties; attr; attr = attr->next)
                if (!strcmp((const char *) attr->name, "value"))
                    value = mp::xml::get_text(attr->children);
                else
                    throw mp::filter::FilterException(
                        "Bad attribute " + std::string((const char *)
                                                       attr->name));
            zorba_record_variable = value;
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
    if (zorba_filename.length() == 0)
        throw mp::filter::FilterException("Missing element filename");
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

