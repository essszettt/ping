SECTION code_user
PUBLIC _zxn_reboot_fastcall

; ==============================================================================
; void zxn_reboot_fastcall(void) __z88dk_fastcall
; ------------------------------------------------------------------------------
; reboot system
; ==============================================================================
_zxn_reboot_fastcall:
  rst 8     ; NextOS call
  db  $F7   ; Hard Reset / Reboot
  ret       ; Should never be reached
