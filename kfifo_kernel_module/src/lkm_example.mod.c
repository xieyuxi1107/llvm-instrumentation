#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xb3753869, "module_layout" },
	{ 0x484ece0d, "noop_llseek" },
	{ 0xa3ee9e12, "remove_proc_entry" },
	{ 0xf23fcb99, "__kfifo_in" },
	{ 0xdb760f52, "__kfifo_free" },
	{ 0xdb59a12d, "proc_create" },
	{ 0x139f2189, "__kfifo_alloc" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x4578f528, "__kfifo_to_user" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "3E921D2D6272F6BF2876799");
