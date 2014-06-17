%define idmetaversion %(. ./IDMETA; printf $VERSION )
Summary: Metaproxy XQuery module
Name: mp-xquery
Version: %{idmetaversion}
Release: 1.indexdata
BuildRequires: gcc gcc-c++ pkgconfig
BuildRequires: libmetaproxy6-devel >= 1.4.0
BuildRequires: wget
BuildRequires: zorba
License: proprietary
Group: Applications/Internet
Vendor: Index Data ApS <info@indexdata.dk>
Source: mp-xquery-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Packager: Adam Dickmeiss <adam@indexdata.dk>
URL: http://www.indexdata.com/mp-xquery

%description
Record conversion module using the Zorba XQuery library

Requires: metaproxy6
Requires: libmetaproxy6
Requires: zorba

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
cd src
make \
	ZORBA=/opt/zorba
	OPT_FLAGS="-g -O" \
	YAZ_CONFIG=/usr/bin/yaz-config \
	MP_CONFIG=/usr/bin/metaproxy-config

%install
cd src
make DESTDIR=${RPM_BUILD_ROOT} libdir=%{_libdir} install

%clean
rm -fr ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%{_libdir}/mp-xquery/*
