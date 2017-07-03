.686
.XMM
.MODEL FLAT,C
.CODE

TestSKZ7Param STRUCT
	Count DWORD ?
	RegBefore	DWORD 8 DUP (?)
	RegAfter	DWORD 8 DUP (?)
	InitValue	BYTE 4 DUP (?)
TestSKZ7Param ENDS



TestSKZ7	PROC
	push	ebx
	push	esi
	push	edi
	
	mov		esi, [esp+16]

	mov		edi, 01010101h
	xor		eax, eax
	xor		ebx, ebx
	xor		ecx, ecx
	xor		edx, edx
	mov		al, [esi].TestSKZ7Param.InitValue[0]
	mov		bl, [esi].TestSKZ7Param.InitValue[1]
	mov		cl, [esi].TestSKZ7Param.InitValue[2]
	mov		dl, [esi].TestSKZ7Param.InitValue[3]	
	imul	eax, edi
	imul	ebx, edi 
	imul	ecx, edi 
	imul	edx, edi 

	mov		[esi].TestSKZ7Param.RegBefore[4*0], eax
	mov		[esi].TestSKZ7Param.RegBefore[4*1], ebx
	mov		[esi].TestSKZ7Param.RegBefore[4*2], ecx
	mov		[esi].TestSKZ7Param.RegBefore[4*3], edx
	mov		[esi].TestSKZ7Param.RegBefore[4*4], edi
	mov		[esi].TestSKZ7Param.RegBefore[4*5], esi	
	mov		[esi].TestSKZ7Param.RegBefore[4*6], esp
	mov		[esi].TestSKZ7Param.RegBefore[4*7], ebp	

	mov		edi, [esi].TestSKZ7Param.Count;
STATE_BEGIN:
	add		ah, bh
	add		ch, dh
	xor		dh, ah
	xor		bh, ch
	rol		ah, 1
	rol		ch, 2
	rol		dh, 3
	rol		bh, 4
	dec		edi
	jnz		STATE_BEGIN

	mov		edi, [esi].TestSKZ7Param.Count;
STATE_BEGIN2:
	add		al, bl
	add		cl, dl
	xor		dl, al
	xor		bl, cl
	rol		al, 1
	rol		cl, 2
	rol		dl, 3
	rol		bl, 4
	dec		edi
	jnz		STATE_BEGIN2

	mov		[esi].TestSKZ7Param.RegAfter[4*0], eax
	mov		[esi].TestSKZ7Param.RegAfter[4*1], ebx
	mov		[esi].TestSKZ7Param.RegAfter[4*2], ecx
	mov		[esi].TestSKZ7Param.RegAfter[4*3], edx
	mov		[esi].TestSKZ7Param.RegAfter[4*4], edi
	mov		[esi].TestSKZ7Param.RegAfter[4*5], esi	
	mov		[esi].TestSKZ7Param.RegAfter[4*6], esp
	mov		[esi].TestSKZ7Param.RegAfter[4*7], ebp

	pop		edi	
	pop		esi
	pop		ebx
	ret
TestSKZ7	ENDP


END