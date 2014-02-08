#define CONFIG_EXTRA_LDFLAGS "-static -Xlinker -z -Xlinker muldefs -nostdlib ${SYSROOT}/usr/lib/crtbegin_static.o ${SYSROOT}/usr/lib/crtend_android.o -L${SYSROOT}/usr/lib"
