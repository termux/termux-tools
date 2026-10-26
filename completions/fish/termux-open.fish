complete -c termux-open -l send -d "Share the file for sending"
complete -c termux-open -l view -d "Share the file for viewing"
complete -c termux-open -l chooser -d "Always show an app chooser"
complete -c termux-open -l content-type -x -d "Specify content type"
complete -c termux-open -s h -l help -d "Show usage information"

complete -c xdg-open -w termux-open
