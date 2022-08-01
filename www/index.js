


let file = undefined

function onOpenImage() {
    const input = document.createElement('input');
    input.type = 'file';
    document.getElementById("mirrored_image").src = ""
    input.onchange = () => {
        if (input.files[0] === undefined)
            return;
        const readerAsUrl = new FileReader()
        readerAsUrl.onload = e => { document.getElementById("original_image").src = e.target.result }
        readerAsUrl.readAsDataURL(input.files[0]);

        const readerAsArray = new FileReader()
        readerAsArray.onload = e => { file = e.target.result }
        readerAsArray.readAsArrayBuffer(input.files[0]);

    }
    input.click()
}

function onMirrorImage() {
    fetch("http://127.0.0.1:8080/mirror", {
        body: file,
        mode: 'cors',
        method: 'POST'
    })
        .then(response => {
            if (!response.ok)
                alert("Oops, something went wrong")
            else
                response.arrayBuffer().then(buffer => {
                    document.getElementById("mirrored_image").src = URL.createObjectURL(new Blob([buffer]));
                })
        }).catch(() => alert("Oops, something went wrong"))
}


