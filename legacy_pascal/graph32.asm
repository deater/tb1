
.Model Medium,Pascal

.CODE

PUBLIC cls32,flipd320,flipd240,flipd50

cls32  PROC FAR pascal

.386

push di
cld

mov ax, [bp+6]
mov es, ax
xor di,di
mov bl,[bp+8]
mov bh,bl
mov ax,bx
shl eax,16
mov ax,bx
mov cx,320*200/4
rep stosd
pop di
ret

cls32  ENDP


;procedure flip32(source,dest:Word);
;This copies the entire screen at "source" to destination

flipd320 PROC FAR PASCAL

.386

    push    ds
    mov     ax, [bp+6]
    mov     es, ax
    mov     ax, [bp+8]
    mov     ds, ax
    xor     si, si
    xor     di, di
    mov     cx, 16000
    rep     movsd
    pop     ds
ret
flipd320  ENDP

;procedure flipd240(hm,va,va2:word);
;label gus;
;  { This copies 240 columns from vaddr2 to vaddr }

flipd240 PROC FAR PASCAL

.386

    push    ds
    mov     ax, [bp+8]
    mov     es, ax
    mov     ax, [bp+6]

    mov     bx,[bp+10]
    mov     cx,bx
    shl     bx,4
    sub     bx,cx
    add     ax,bx

    mov     ds, ax
    xor     si,si
    xor     di, di
  mov ax,200
  gus:
    mov     cx, 60
    rep     movsd
    add     di,80

    dec ax
  jnz gus
  pop     ds
ret
flipd240 ENDP


;procedure flip50(fromwhere,off1,whereto,off2:word);
;label gus;
;  { This copies 240 columns from vaddr2 to vaddr }

flipd50 PROC FAR PASCAL

.386
    push    ds
    mov     ax, [bp+8]      ;whereto
    add     ax, [bp+6]       ;off2
    mov     es, ax
    mov     ax, [bp+12]      ;fromwhere
    add     ax,[bp+10]        ;off1
    mov     ds, ax
    xor     si,si
    xor     di, di
  mov ax,50
  gus2:
    mov     cx, 60
    rep     movsd
  dec ax
  jnz gus2

    pop     ds
ret

flipd50 ENDP

END