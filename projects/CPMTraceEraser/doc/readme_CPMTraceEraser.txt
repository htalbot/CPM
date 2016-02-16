CPMTraceEraser
=========================================================

    To include wxWidgets into CPMTraceEraser...

        - In src/CPMTraceEraser.mpc, uncomment...

            macros += CPMPROCESS_WX

            macros += wxHAS_MODE_T // If the application uses ACE or another library that defines mode_t

            unicode = 1  // if wxWidgets is unicode

        - In features file CPMTRACEERASER_PRJ_ROOT/features, add this line:
            wxwindows=1

        - Set env vars
            CPMTRACEERASER_ABC_PORT = 10199 (where ABC is an ID for that process instance and 10199 is an example of IP port)

        - Generate project/solution/makefile (dpo.pl)

            - Add wxcore_dpo.mpb as dependency (or more)

        - When using CPMTraceEraser with CPMM, defines port as "$(CPMTRACEERASER_ABC_PORT)" in cpm_<dev>.cfg (where dev is specialized settings)
          (to be coherent with gui)

        - Generate project/solution/makefile with dpo.pl
            (if dpo.pl is already running, restart it. Each time we
             edit the features file, we need to orestart dpo.pl).


