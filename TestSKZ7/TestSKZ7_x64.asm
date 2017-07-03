.CODE




TestSKZ7Param STRUCT
	Count QWORD ?
	RegBefore	QWORD 16 DUP (?)
	RegAfter	QWORD 16 DUP (?)
	InitValue	BYTE 4 DUP (?)
TestSKZ7Param ENDS



TestSKZ7	PROC
	push	rbx
	push	rsi
	push	rdi
	
	mov		rsi, rcx

	mov		r8, 0101010101010101h
	xor		rax, rax
	xor		rbx, rbx
	xor		rcx, rcx
	xor		rdx, rdx
	mov		al, [rsi].TestSKZ7Param.InitValue[0]
	mov		bl, [rsi].TestSKZ7Param.InitValue[1]
	mov		cl, [rsi].TestSKZ7Param.InitValue[2]
	mov		dl, [rsi].TestSKZ7Param.InitValue[3]	
	imul	rax, r8
	imul	rbx, r8 
	imul	rcx, r8 
	imul	rdx, r8 

	mov		[rsi].TestSKZ7Param.RegBefore[8*0], rax
	mov		[rsi].TestSKZ7Param.RegBefore[8*1], rbx
	mov		[rsi].TestSKZ7Param.RegBefore[8*2], rcx
	mov		[rsi].TestSKZ7Param.RegBefore[8*3], rdx
	mov		[rsi].TestSKZ7Param.RegBefore[8*4], rdi
	mov		[rsi].TestSKZ7Param.RegBefore[8*5], rsi	
	mov		[rsi].TestSKZ7Param.RegBefore[8*6], rsp
	mov		[rsi].TestSKZ7Param.RegBefore[8*7], rbp
	mov		[rsi].TestSKZ7Param.RegBefore[8*8], r8
	mov		[rsi].TestSKZ7Param.RegBefore[8*9], r9
	mov		[rsi].TestSKZ7Param.RegBefore[8*10], r10
	mov		[rsi].TestSKZ7Param.RegBefore[8*11], r11
	mov		[rsi].TestSKZ7Param.RegBefore[8*12], r12
	mov		[rsi].TestSKZ7Param.RegBefore[8*13], r13
	mov		[rsi].TestSKZ7Param.RegBefore[8*14], r14
	mov		[rsi].TestSKZ7Param.RegBefore[8*15], r15	

	mov		rdi, [rsi].TestSKZ7Param.Count;
STATE_BEGIN:
	add		ah, bh
	add		ch, dh
	xor		dh, ah
	xor		bh, ch
	rol		ah, 1
	rol		ch, 2
	rol		dh, 3
	rol		bh, 4
	dec		rdi
	jnz		STATE_BEGIN

	mov		rdi, [rsi].TestSKZ7Param.Count;
STATE_BEGIN2:
	add		al, bl
	add		cl, dl
	xor		dl, al
	xor		bl, cl
	rol		al, 1
	rol		cl, 2
	rol		dl, 3
	rol		bl, 4
	dec		rdi
	jnz		STATE_BEGIN2

	mov		[rsi].TestSKZ7Param.RegAfter[8*0], rax
	mov		[rsi].TestSKZ7Param.RegAfter[8*1], rbx
	mov		[rsi].TestSKZ7Param.RegAfter[8*2], rcx
	mov		[rsi].TestSKZ7Param.RegAfter[8*3], rdx
	mov		[rsi].TestSKZ7Param.RegAfter[8*4], rdi
	mov		[rsi].TestSKZ7Param.RegAfter[8*5], rsi	
	mov		[rsi].TestSKZ7Param.RegAfter[8*6], rsp
	mov		[rsi].TestSKZ7Param.RegAfter[8*7], rbp
	mov		[rsi].TestSKZ7Param.RegAfter[8*8], r8
	mov		[rsi].TestSKZ7Param.RegAfter[8*9], r9
	mov		[rsi].TestSKZ7Param.RegAfter[8*10], r10
	mov		[rsi].TestSKZ7Param.RegAfter[8*11], r11
	mov		[rsi].TestSKZ7Param.RegAfter[8*12], r12
	mov		[rsi].TestSKZ7Param.RegAfter[8*13], r13
	mov		[rsi].TestSKZ7Param.RegAfter[8*14], r14
	mov		[rsi].TestSKZ7Param.RegAfter[8*15], r15

	pop		rdi	
	pop		rsi
	pop		rbx
	ret
TestSKZ7	ENDP


END