#!/bin/bash
# check-vasm.sh <path to vasmm68k_mot>
# Assembles small cases and compares the bytes. "fix" cases fail on stock 2.0g,
# "ctrl" cases must pass on stock and patched builds alike.
V=${1:?usage: check-vasm.sh <path to vasmm68k_mot>}
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
fail=0
c() { # kind name cpu expected-hex source-line [source-line]
  local kind=$1 name=$2 cpu=$3 want=$4 got; shift 4
  { echo "	section	code,code"; printf '%s\n' "$@"
    printf '	rts\ntable:	dc.b	1,2,3,4\ndest:	dc.l	0\n'; } > "$T/t.s"
  if ! "$V" -quiet "$cpu" -Fbin -o "$T/t.bin" "$T/t.s" >"$T/log" 2>&1; then
    echo "FAIL $kind $name: assembler error"; head -3 "$T/log"; fail=$((fail+1)); return
  fi
  got=$(od -An -v -tx1 "$T/t.bin" | tr -d ' \n'); got=${got:0:${#want}}
  if [ "$got" = "$want" ]; then echo "PASS $kind $name ($want)"
  else echo "FAIL $kind $name: got $got, want $want"; fail=$((fail+1)); fi
}
#  PC-relative destinations are opt-in (OPT OA+) on -m68080
c fix  "move.b full-format source,label stays abs.l" -m68080 13f001b0 "	move.b	(table.l,d0.w),dest"
c fix  "addq.l #1,label stays abs.l"                 -m68080 52b9     "	addq.l	#1,dest"
c ctrl "OPT OA+ still gives (d16,PC)"                -m68080 15f001b0 "	opt	oa+" "	move.b	(table.l,d0.w),dest"
#  sub.l/subi.l #w must not become addiw.l #-w (inverted C and X)
c fix  "sub.l #1000,d1 stays sub"                    -m68080 92bc000003e8 "	sub.l	#1000,d1"
c fix  "subi.l #1000,d1 stays subi"                  -m68080 0481000003e8 "	subi.l	#1000,d1"
c ctrl "add.l #1000,d1 still becomes addiw.l"        -m68080 06c103e8     "	add.l	#1000,d1"
#  move.l #-128..127,Dn becomes moveq
c fix  "move.l #5,d0 becomes moveq"                  -m68080 7005 "	move.l	#5,d0"
c fix  "move.l #-128,d3 becomes moveq"               -m68080 7680 "	move.l	#-128,d3"
c ctrl "move.l #128,d4 stays move.l"                 -m68080 283c00000080 "	move.l	#128,d4"
#  other CPUs are unchanged
c ctrl "-m68020 add.l stays add.l"                   -m68020 d2bc000003e8 "	add.l	#1000,d1"
c ctrl "-m68020 label destination stays abs.l"       -m68020 13f001b0 "	move.b	(table.l,d0.w),dest"
[ $fail = 0 ] && echo "ALL PASS" || echo "$fail FAILED"
exit $((fail>0))
