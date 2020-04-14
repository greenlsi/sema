ltl spec1 {
    [] (((state == 0) && button) -> <> (state == 1)) &&
    [] (((state == 1) && button) -> <> (state == 0))
}
ltl spec2 {
    [] (((state == 0) && ([] (!button))) -> [] (state == 0)) &&
    [] (((state == 1) && ([] (!button))) -> [] (state == 1))
}

bit button;
byte state;

active proctype lampara_fsm () {
    state = 0;
    do
    :: (state == 0) -> atomic {
        if
        :: button -> state = 1; button = 0
        fi
    }
    :: (state == 1)  -> atomic {
        if
        :: button -> state = 0; 
        fi
    }
    od
}

active proctype entorno () {
    do
    ::	if
        :: button = 1
        :: (! button) -> skip
	fi;
	printf ("button = %d, state = %d\n", button, state)
    od
}
