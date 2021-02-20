((nil . ((eval . (progn
                   (setq flycheck-clang-include-path
                         (list (expand-file-name "~/code/rona/ext/")))

                   (defvar context-mode-map (make-sparse-keymap)
                     "Keymap while context-mode is active.")
                   (define-minor-mode context-mode
                     "A temporary minor mode to be activated only specific to a buffer."
                     nil
                     :lighter " [C-c m]-Context"
                     context-mode-map)
                   (context-mode 1)

                   (defhydra hydra-context (:color pink :exit t)
                    "
                    Rona Make Commands
                    ----------------------------------------------------
                    Build   : _g_: guest _h_: host
                    Execute : _r_: run   _t_: test
                    Misc    : _n_: tags  _f_: format
                    "
                     ("g" (compile "make guest -C ~/code/rona"))
                     ("h" (compile "make host  -C ~/code/rona"))
                     ("r" (compile "make run   -C ~/code/rona"))
                     ("t" (compile "make test  -C ~/code/rona"))
                     ("n" (compile "make tags  -C ~/code/rona"))
                     ("f" (compile "make fmt   -C ~/code/rona"))
                     ("RET" nil "quit" :color blue))

                   (define-key context-mode-map (kbd "C-c m") 'hydra-context/body))))))
