#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x59caa4c3, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x268f87ce, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x81ba77be, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xf4520e65, __VMLINUX_SYMBOL_STR(parport_register_driver) },
	{ 0x1ab099dd, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x29c6165c, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x111d6b33, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0xd7bd463f, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xf947d881, __VMLINUX_SYMBOL_STR(parport_register_device) },
	{ 0x2dfaefdb, __VMLINUX_SYMBOL_STR(parport_release) },
	{ 0xe7194c8e, __VMLINUX_SYMBOL_STR(parport_claim_or_block) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=parport";


MODULE_INFO(srcversion, "3022C0B9FE25D0B64F1375C");
