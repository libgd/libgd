Summary:       A graphics library for quick creation of PNG or JPEG images
Name:          gd
Version:       2.3.3
Release:       %{autorelease}
License:       GD
URL:           http://libgd.github.io/
Source0:       https://github.com/libgd/libgd/releases/download/gd-%{version}/libgd-%{version}.tar.xz
 
BuildRequires: freetype-devel
BuildRequires: fontconfig-devel
BuildRequires: gettext-devel
BuildRequires: libjpeg-devel
BuildRequires: libpng-devel
BuildRequires: libtiff-devel
BuildRequires: libwebp-devel
BuildRequires: libimagequant-devel
# Failing test
# https://github.com/libgd/libgd/issues/887
# https://github.com/libgd/libgd/issues/790#issuecomment-982894815
# FAIL: gdimagestringft/gdimagestringft_bbox
#BuildRequires: libraqm-devel
BuildRequires: libavif-devel
BuildRequires: libX11-devel
BuildRequires: libXpm-devel
BuildRequires: zlib-devel
BuildRequires: pkgconfig
BuildRequires: libtool
BuildRequires: perl-interpreter
BuildRequires: perl-generators
BuildRequires: perl(FindBin)
# for fontconfig/basic test
BuildRequires: liberation-sans-fonts
BuildRequires: make
 
 
%description
The gd graphics library allows your code to quickly draw images
complete with lines, arcs, text, multiple colors, cut and paste from
other images, and flood fills, and to write out the result as a PNG or
JPEG file. This is particularly useful in Web applications, where PNG
and JPEG are two of the formats accepted for inline images by most
browsers. Note that gd is not a paint program.
 
 
%package progs
Requires:       %{name}%{?_isa} = %{version}-%{release}
Summary:        Utility programs that use libgd
 
%description progs
The gd-progs package includes utility programs supplied with gd, a
graphics library for creating PNG and JPEG images.
 
 
%package devel
Summary:  The development libraries and header files for gd
Requires: %{name}%{?_isa} = %{version}-%{release}
Requires: freetype-devel%{?_isa}
Requires: fontconfig-devel%{?_isa}
Requires: libjpeg-devel%{?_isa}
Requires: libpng-devel%{?_isa}
Requires: libtiff-devel%{?_isa}
Requires: libwebp-devel%{?_isa}
Requires: libX11-devel%{?_isa}
Requires: libXpm-devel%{?_isa}
Requires: zlib-devel%{?_isa}
Requires: libimagequant-devel%{?_isa}
#Requires: libraqm-devel
Requires: libavif-devel
 
 
%description devel
The gd-devel package contains the development libraries and header
files for gd, a graphics library for creating PNG and JPEG graphics.
 
 
%prep
%autosetup  -n libgd-%{version}
 
: $(perl config/getver.pl)
 
: regenerate autotool stuff
if [ -f configure ]; then
   libtoolize --copy --force
   autoreconf -vif
else
   ./bootstrap.sh
fi
 
 
%build
# Provide a correct default font search path
CFLAGS="-std=gnu17 $RPM_OPT_FLAGS -DDEFAULT_FONTPATH='\"\
/usr/share/fonts/bitstream-vera:\
/usr/share/fonts/dejavu:\
/usr/share/fonts/default/Type1:\
/usr/share/X11/fonts/Type1:\
/usr/share/fonts/liberation\"'"
 
%ifarch %{ix86}
# see https://github.com/libgd/libgd/issues/242
CFLAGS="$CFLAGS -msse -mfpmath=sse"
%endif
 
%ifarch aarch64 ppc64 ppc64le s390 s390x x86_64 riscv64
# workaround for https://bugzilla.redhat.com/show_bug.cgi?id=1359680
export CFLAGS="$CFLAGS -ffp-contract=off"
%endif
 
%configure \
    --enable-gd-formats \
    --with-tiff=%{_prefix} \
    --disable-rpath
make %{?_smp_mflags}
 
 
%install
make install INSTALL='install -p' DESTDIR=%{buildroot}
rm -f %{buildroot}/%{_libdir}/libgd.la
rm -f %{buildroot}/%{_libdir}/libgd.a
 
 
%check
# Workaround to https://github.com/libgd/libgd/issues/763
export TMPDIR=/tmp
 
# Upstream test suite
make check
 
 
%files
%license COPYING
%{_libdir}/libgd.so.3{,.*}
 
%files progs
%{_bindir}/bdftogd
%{_bindir}/gdcmpgif
%{_bindir}/gdtopng
%{_bindir}/pngtogd
%{_bindir}/webpng
%{_bindir}/gdparttopng
%{_bindir}/gd2topng
%{_bindir}/pngtogd2
%{_bindir}/annotate
%{_bindir}/gd2copypal
%{_bindir}/gd2togif
%{_bindir}/giftogd2
 
%files devel
%{_includedir}/gd.h
%{_includedir}/gdfx.h
%{_includedir}/gd_io.h
%{_includedir}/gdcache.h
%{_includedir}/gdfontg.h
%{_includedir}/gdfontl.h
%{_includedir}/gdfontmb.h
%{_includedir}/gdfonts.h
%{_includedir}/gdfontt.h
%{_includedir}/gd_color_map.h
%{_includedir}/gd_errors.h
%{_includedir}/gdpp.h
%{_libdir}/libgd.so
%{_libdir}/pkgconfig/gdlib.pc

%changelog
%autochangelog
