include $(TOPDIR)/rules.mk

PKG_NAME:=gluon-private-wifi

include ../gluon.mk

define Package/gluon-private-wifi
  TITLE:=Provides a virtual access point for the nodes WAN network 
  DEPENDS:=+gluon-core
endef

$(eval $(call BuildPackageGluon,gluon-private-wifi))
