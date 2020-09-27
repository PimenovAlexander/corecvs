
function recursive_Ajax(url, func, options = {}) {
    options.recursive = true;
    return send_Ajax(url, func, options);
}

function send_Ajax(url, func, options = {}) {
    const xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);

    let delay = (options.failureDelay) ? options.failureDelay : 1000;

    // Closure variables

    let received = false;

    xhr.onload = function() {
        if (received) return; // Request has already been sent again because of connection problems

        if (this.status === 200) {
            received = true;
            func(this.responseText);

            if (options.recursive) {
                send_Ajax(url, func, options);
            }
        } else {
            if (options.error) {
                options.error(this.responseText);
            }
        }
    };

    setInterval(_ => {
        if (!received) {
            received = true;
            send_Ajax(url, func, options);
        }
    }, delay);

    xhr.send();
}
