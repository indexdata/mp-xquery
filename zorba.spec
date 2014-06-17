Summary: XQuery Processor
Name: zorba
Version: 3.0
Release: 1.indexdata
BuildRequires: gcc gcc-c++ pkgconfig
BuildRequires: wget
BuildRequires: xerces-c-devel
BuildRequires: libcurl-devel
BuildRequires: libedit-devel
BuildRequires: libxslt-devel
BuildRequires: bison
License: Apache
Group: Applications/Internet
Vendor: Index Data ApS <info@indexdata.dk>
Source: zorba-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Packager: Adam Dickmeiss <adam@indexdata.dk>
URL: http://www.indexdata.com/mp-xquery

%description
Zorba - The XQuery Processor developed by the FLWOR Foundation

Requires: libcurl
Requires: xerces-c

%prep
%setup

%build
mkdir build
cd build
/opt/cmake/bin/cmake \
	-Wno-dev \
	-D CMAKE_INSTALL_PREFIX=/opt/zorba \
	-D ZORBA_SUPPRESS_SWIG:BOOL=ON \
	..
cd ..

%install
cd build
rm -fr ${RPM_BUILD_ROOT}
make DESTDIR=${RPM_BUILD_ROOT} install
cd ..

%clean
rm -fr ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
/opt
