complete -c chsh -f
complete -c chsh -s h -l help -d "Show usage information"

function __fish_chsh_shells
    set -l known_shells \
        ash \
        bash \
        beanshell \
        bsh \
        cicada \
        csh \
        dash \
        eltclsh \
        elvish \
        etsh \
        fish \
        hilbish \
        ksh \
        loksh \
        mksh \
        nu \
        osh \
        pwsh \
        rc \
        sh \
        tcsh \
        tsh \
        xonsh \
        ysh \
        zsh

    for sh in $known_shells
        if type -q $sh
            echo $sh
        end
    end
end

complete -c chsh -s s -x -a "(__fish_chsh_shells)" -d "Login shell"
