"use strict";

(function (){

    function toggleCheckbox(element) {
        var xhr = new XMLHttpRequest();
        if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
        else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
        xhr.send();
    }

    function button_send_back(elementId){

        var rque = new XMLHttpRequest();
        var ibtnId = parseInt(elementId);
        var itmp = ibtnId-11;
        var elId = itmp.toString();

        if(itmp > 0){
            var elIdinput = (itmp+101).toString();
            var str = document.getElementById(elIdinput).value;
            rque.open("GET", "/savename?rindex="+elId+"&rname="+str, true);
            rque.send();
        }
        else{
            var str = document.getElementById('101').value;
            rque.open("GET", "/savename?rindex="+elId+"&rname="+str, true);
            rque.send();
        }
    }

})();