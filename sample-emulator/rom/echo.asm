    in          ; 00: 01        read input from keyboard and place it in A
    bz 0        ; 01: 02 00     if A == 0, jump to address 0 (start of program)
    out         ; 03: 03        print whatever character is in A
    jp  0       ; 04: 04 00     jump to address 0 (start of program)