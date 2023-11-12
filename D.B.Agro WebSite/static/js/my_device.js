setInterval(add_current_data, 1000);
function add_current_data(){
    $.ajax({
        type : "POST",
        url : "/my_device",
        data : {
            "ok" : "ok"
        },
        success:function(data){
            var table = document.getElementById("jadval");
            var row = table.insertRow(1);
            table.deleteRow(10);
            var cell1 = row.insertCell(0);
            cell1.innerHTML = data["date"];
            var cell2 = row.insertCell(1);
            cell2.innerHTML = data["havonamlik"];
            var cell3 = row.insertCell(2);
            cell3.innerHTML = data["temperature"];
            var cell4 = row.insertCell(3);
            cell4.innerHTML = data["tuproqnamlik"];
            var cell5 = row.insertCell(4);
            cell5.innerHTML = data["suvbormi"];
            var cell6 = row.insertCell(5);
            cell6.innerHTML = data["motoryoniqmi"];
        }
      });
};