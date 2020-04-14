ltl spec {
    [] ((s[0] != 2) ||
        (s[1] != 2) ||
        (s[2] != 2) ||
        (s[3] != 0) ||
        (s[4] != 1) ||
        (s[5] != 1) ||
        (s[6] != 1))
}
ltl spec2 {
	s[0] != 3
}

byte s[7] = { 1, 1, 1, 0, 2, 2, 2 };

active proctype frogs () {
    do
    :: if
       :: (s[0] == 1) && (s[1] == 0) -> s[0] = 0; s[1] = 1
       :: (s[1] == 1) && (s[2] == 0) -> s[1] = 0; s[2] = 1
       :: (s[2] == 1) && (s[3] == 0) -> s[2] = 0; s[3] = 1
       :: (s[3] == 1) && (s[4] == 0) -> s[3] = 0; s[4] = 1
       :: (s[4] == 1) && (s[5] == 0) -> s[4] = 0; s[5] = 1
       :: (s[5] == 1) && (s[6] == 0) -> s[5] = 0; s[6] = 1

       :: (s[0] == 1) && (s[1] != 0) && (s[2] == 0) -> s[0] = 0; s[2] = 1
       :: (s[1] == 1) && (s[2] != 0) && (s[3] == 0) -> s[1] = 0; s[3] = 1
       :: (s[2] == 1) && (s[3] != 0) && (s[4] == 0) -> s[2] = 0; s[4] = 1
       :: (s[3] == 1) && (s[4] != 0) && (s[5] == 0) -> s[3] = 0; s[5] = 1
       :: (s[4] == 1) && (s[5] != 0) && (s[6] == 0) -> s[4] = 0; s[6] = 1

       :: (s[1] == 2) && (s[0] == 0) -> s[1] = 0; s[0] = 2
       :: (s[2] == 2) && (s[1] == 0) -> s[2] = 0; s[1] = 2
       :: (s[3] == 2) && (s[2] == 0) -> s[3] = 0; s[2] = 2
       :: (s[4] == 2) && (s[3] == 0) -> s[4] = 0; s[3] = 2
       :: (s[5] == 2) && (s[4] == 0) -> s[5] = 0; s[4] = 2
       :: (s[6] == 2) && (s[5] == 0) -> s[6] = 0; s[5] = 2

       :: (s[2] == 2) && (s[1] != 0) && (s[0] == 0) -> s[2] = 0; s[0] = 2
       :: (s[3] == 2) && (s[2] != 0) && (s[1] == 0) -> s[3] = 0; s[1] = 2
       :: (s[4] == 2) && (s[3] != 0) && (s[2] == 0) -> s[4] = 0; s[2] = 2
       :: (s[5] == 2) && (s[4] != 0) && (s[3] == 0) -> s[5] = 0; s[3] = 2
       :: (s[6] == 2) && (s[5] != 0) && (s[4] == 0) -> s[6] = 0; s[4] = 2
       fi ;

       int i;
       for (i : 0 .. 6) {
           printf ("%d", s[i])
       };
       printf ("\n");
    od
}