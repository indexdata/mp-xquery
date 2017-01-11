%define idmetaversion %(. ./IDMETA; printf $VERSION )
Summary: Metaproxy XQuery module
Name: mp-xquery
Version: %{idmetaversion}
Release: 2.indexdata
BuildRequires: gcc gcc-c++ pkgconfig
BuildRequires: libmetaproxy6-devel >= 1.4.0
BuildRequires: idzorba
BuildRequires: libxslt
BuildRequires: docbook-style-xsl
License: GPL
Group: Applications/Internet
Vendor: Index Data ApS <info@indexdata.dk>
Source: mp-xquery-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Packager: Adam Dickmeiss <adam@indexdata.dk>
URL: http://www.indexdata.com/mp-xquery
Requires: metaproxy6
Requires: libmetaproxy6
Requires: idzorba

%description
Record conversion module using the Zorba XQuery library

%post
if [ -d /usr/lib64/metaproxy6/modules ]; then
       	if [ ! -e /usr/lib64/metaproxy6/modules/metaproxy_filter_xquery.so ]; then
		ln -s /usr/lib64/mp-xquery/metaproxy_filter_xquery.so /usr/lib64/metaproxy6/modules
	fi
fi
if [ -f /var/run/metaproxy.pid ]; then
	/sbin/service metaproxy restart
fi
%preun
if [ $1 = 0 ]; then
	if [ -f /var/run/metaproxy.pid ]; then
		/sbin/service metaproxy restart
	fi
fi

%postun
if [ $1 = 0 ]; then
	rm -f /usr/lib64/metaproxy6/modules/metaproxy_filter_xquery.so
fi

%prep
%setup

%build
make \
	ZORBA=/opt/idzorba \
	MP_CONFIG=/usr/bin/metaproxy-config

%install
make DESTDIR=${RPM_BUILD_ROOT} prefix=%{_prefix} libdir=%{_libdir} install

%clean
rm -fr ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%{_libdir}/mp-xquery/*
%{_mandir}/man3/xquery.*
%{_datadir}/mp-xquery
