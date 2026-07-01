Name:       com.samsung.dali-simple-test
Summary:    DALi simple test application
Version:    0.1.0
Release:    1
Group:      System/Libraries
License:    Apache-2.0
URL:        https://review.tizen.org/git/
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

BuildRequires:  cmake
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(dali2-core)
BuildRequires:  pkgconfig(dali2-adaptor)
BuildRequires:  pkgconfig(dali2-toolkit)
BuildRequires:  dali2-integration-devel
BuildRequires:  dali2-adaptor-integration-devel
BuildRequires:  dali2-toolkit-integration-devel

%description
DALi simple test application for focused accessibility refactoring validation.

%prep
%setup -q

%define dali_app_ro_dir       %TZ_SYS_RO_APP/com.samsung.dali-simple-test/
%define dali_xml_file_dir     %TZ_SYS_RO_PACKAGES
%define dali_app_exe_dir      %{dali_app_ro_dir}/bin/

%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -O2"
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed"

cd %{_builddir}/%{name}-%{version}/build/tizen

cmake -DDALI_APP_DIR=%{dali_app_exe_dir} \
%if 0%{?enable_debug}
      -DCMAKE_BUILD_TYPE=Debug \
%endif
      -DTIZEN:BOOL=ON \
      .

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
cd build/tizen
%make_install DALI_APP_DIR=%{dali_app_exe_dir}

mkdir -p %{buildroot}%{dali_xml_file_dir}
cp -f %{_builddir}/%{name}-%{version}/%{name}.xml %{buildroot}%{dali_xml_file_dir}

%post
/sbin/ldconfig
exit 0

%postun
/sbin/ldconfig
exit 0

%files
%manifest com.samsung.dali-simple-test.manifest
%defattr(-,root,root,-)
%{dali_app_exe_dir}/dali-simple-test
%{dali_xml_file_dir}/%{name}.xml
