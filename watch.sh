while true; do inotifywait -e close_write --quiet --recursive src; make; done
