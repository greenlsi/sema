ltl spec {
    [] ! ((cstate == green) && (pstate == crossing))
}

byte count;
bit pedestrian;
bit sigR, sigG, sigY;

mtype = { red, green, yellow, pending };
mtype cstate;

active proctype tlc () {
       count = 0;
       pedestrian = 0;
       sigR = 0; sigG = 0; sigY = 0;
       cstate = red;

       do
       :: (cstate == red) -> atomic {
                 if
                 :: (count >= 60) -> cstate = green; count = 0; sigG = 1
                 :: count = count + 1
                 fi
       }
       :: (cstate == green) -> atomic {
                 if
                 :: (pedestrian && (count >= 60)) ->
                         cstate = yellow; count = 0; sigY = 1
                 :: (pedestrian && (count < 60)) ->
                         cstate = pending; count = count + 1
                 :: count = count + 1
                 fi
       }
       :: (cstate == yellow) -> atomic {
                 if
                 :: (count >= 5) -> cstate = red; count = 0; sigR = 1
                 :: count = count + 1
                 fi
       }
       :: (cstate == pending) -> atomic {
                 if
                 :: (count >= 60) -> cstate = yellow; count = 0; sigY = 1
                 :: count = count + 1
                 fi
       }
       od
}

mtype = { crossing, none, waiting };
mtype pstate;

active proctype pmodel () {
    pstate = crossing;

    do
    :: if
       :: (pstate == crossing) -> atomic {
                 if
                 :: sigG -> pstate = none; sigG = 0
                 fi
       }
       :: (pstate == none) -> atomic {
                 if
                 :: pstate = waiting; pedestrian = 1
                 :: skip
                 fi
       }
       :: (pstate == waiting) -> atomic {
                 if
                 :: sigR -> pstate = crossing; sigR = 0
                 fi
       }
       :: sigY -> sigY = 0
       fi;
       printf ("RGY %d%d%d, cstate = %d, pstate = %d\n",
               sigR, sigG, sigY, cstate, pstate);
    od
}
