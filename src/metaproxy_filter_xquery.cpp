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

namespace mp = metaproxy_1;
namespace yf = mp::filter;
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
        };
    }
}

void yf::XQuery::start() const
{
}

void yf::XQuery::stop(int signo) const
{
}

void yf::XQuery::process(Package &package) const
{
    package.move();
}

void yf::XQuery::configure(const xmlNode * ptr, bool test_only,
                           const char *path)
{
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

