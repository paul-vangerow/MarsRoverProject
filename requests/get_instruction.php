<?php
    include 'db_connect.php';
    include 'jsonphp.php';

    // connect to the database
    $map_id = "map_".getMap();

    $conn = Database::connect($map_id);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // first call to the database to get the unexecuted instructions
    $sql = "SELECT instruction, val FROM instructions WHERE executed=false";
    $qr = $conn->query($sql);
    foreach($qr as $row){
        $instr = $row['instruction'];
        $val = $row['val'];
        echo "$instr\t$val";
        break;
    }

    //second call to the database to mark the instructions as executed
    $time = date("Y-m-d H:i:s");
    $sql = "UPDATE instructions 
        SET executed=true, execute_time='$time'
        WHERE executed=false";
    $qr = $conn->query($sql);   
    
    
    Database::disconnect();

?>