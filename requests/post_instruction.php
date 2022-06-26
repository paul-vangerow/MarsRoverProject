<?php

    include 'db_connect.php';

    $INSTR = $_POST["instr"];
    $VAL = $_POST["val"];
    $time = date("Y-m-d H:i:s");
    
    if(isset($_POST['executed'])){
        $executed = $_POST['executed'];
    }
    else{
        $executed = 0;
    }

    // connect to the database
    $conn = Database::connect('test');
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    $sql = "INSERT INTO test_instr(instr, val, executed, order_time, execute_time)
            VALUES('$INSTR', '$VAL', '$executed', '$time', null)";
    $conn->exec($sql);

    Database::disconnect();
?>