var Video = {
    canvas : document.createElement("canvas"),
    stream : function() {
        this.canvas.width = 500;
        this.canvas.height = 350;
    }
}

function make_visible(){
    document.getElementById("flm").style.visibility = "visible";
}

function make_invisible(){
    document.getElementById("flm").style.visibility = "hidden";
}

function make_visible2(){
    document.getElementById("frm").style.visibility = "visible";
}

function make_invisible2(){
    document.getElementById("frm").style.visibility = "hidden";
}

function make_visible3(){
    document.getElementById("rlm").style.visibility = "visible";
}

function make_invisible3(){
    document.getElementById("rlm").style.visibility = "hidden";
}

function make_visible4(){
    document.getElementById("rrm").style.visibility = "visible";
}

function make_invisible4(){
    document.getElementById("rrm").style.visibility = "hidden";
}

$(document).ready(function () {
    $('select-camera').selectize({
        sortField: 'text' 
    });
});

$(document).ready(function () {
    $('select-controller').selectize({
        sortField: 'text' 
    });
});