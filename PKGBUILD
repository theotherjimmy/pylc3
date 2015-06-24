# This is an example PKGBUILD file. Use this as a start to creating your own,
# and remove these comments. For more information, see 'man PKGBUILD'.
# NOTE: Please fill out the license field for your package! If it is unknown,
# then please put 'unknown'.

# The following guidelines are specific to BZR, GIT, HG and SVN packages.
# Other VCS sources are not natively supported by makepkg yet.

# Maintainer: Your Name <youremail@domain.com>
pkgname=pylc3-git # '-bzr', '-git', '-hg' or '-svn'
pkgrel=1
pkgver=r124.a7e5aae
pkgdesc=""
arch=('any')
url=""
license=('GPL')
groups=()
depends=('boost' 'boost-libs' 'python2')
makedepends=('git' 'boost' 'boost-libs' 'python2')
provides=("${pkgname%-git}")
conflicts=("${pkgname%-git}")
replaces=()
backup=()
options=()
install=
source=("${pkgname}::git+https://github.com/theotherjimmy/pylc3.git")
noextract=()
md5sums=('SKIP')

# Please refer to the 'USING VCS SOURCES' section of the PKGBUILD man page for
# a description of each element in the source array.

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
  mkdir -p ${pkgdir}/usr/lib/python2.7/site-packages/
  install _Linux/pylc3.so ${pkgdir}/usr/lib/python2.7/site-packages/
}
