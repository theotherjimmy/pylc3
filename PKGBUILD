
# Maintainer: Jimmy Brisson <theotherjimmy@gmail.com>
pkgname=python2-pylc3-git
pkgrel=1
pkgver=r157.4542d24
pkgdesc="A python and C++ LC-3 simulator"
arch=('any')
url="http://github.com/theotherjimmy/pylc3"
license=('GPL')
depends=('boost' 'boost-libs' 'python2')
makedepends=('git' 'boost' 'boost-libs' 'python2')
provides=("${pkgname%-git}")
conflicts=("${pkgname%-git}")
source=("${pkgname}::git+https://github.com/theotherjimmy/pylc3.git")
md5sums=('SKIP')

pkgver() {
	cd "$srcdir/${pkgname%-VCS}"
	printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
	cd "$srcdir/${pkgname%-VCS}"
	make
}

check() {
	cd "$srcdir/${pkgname%-VCS}"
	make -k test
}

package() {
	cd "$srcdir/${pkgname%-VCS}"
  make install DESTDIR=${pkgdir}
}
