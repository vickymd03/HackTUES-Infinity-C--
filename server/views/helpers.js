var Video = {
    canvas : document.createElement("canvas"),
    stream : function() {
        this.canvas.width = 500;
        this.canvas.height = 350;
    }
}

function openNav() {
    document.getElementById("mySidenav").style.width = "250px";
    document.getElementById("main").style.marginLeft = "250px";
}
  
function closeNav() {
    document.getElementById("mySidenav").style.width = "0";
    document.getElementById("main").style.marginLeft= "0";
}


$(document).ready(function () {
    $('select').selectize({
        sortField: 'text' 
    });
});