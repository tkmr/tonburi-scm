	.text
.globl _main
_main:
	subl	$12, %esp
	call	L_scheme_entry$stub
	addl	$12, %esp
	ret
.globl _scheme_entry
_scheme_entry:
	subl	$12, %esp
	movl	$0, %eax
	addl	$12, %esp
	ret
	.section __IMPORT,__jump_table,symbol_stubs,self_modifying_code+pure_instructions,5
L_scheme_entry$stub:
	.indirect_symbol _scheme_entry
	hlt ; hlt ; hlt ; hlt ; hlt
	.subsections_via_symbols
