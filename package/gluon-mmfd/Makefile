include $(TOPDIR)/rules.mk

PKG_NAME:=gluon-mmfd

include ../gluon.mk

define Package/gluon-mmfd
  TITLE:=Mesh multicast forwarding daemon - Gluon integration
  DEPENDS:=+mmfd +uc +gluon-core
endef

$(eval $(call BuildPackageGluon,gluon-mmfd))
