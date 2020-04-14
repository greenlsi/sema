ltl spec {
    [] (((state == 0) && button) -> <> (state == 1)) &&
    [] ((state == 1) -> <> (state == 0))
}

#define timeout true

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
        :: button -> state = 0; button = 0
        :: timeout -> state = 0
        fi
    }
    od
}

active proctype entorno () {
    button = 0;
    do
    :: button = 1
    :: (!button) -> skip
    od
}
