<?php

    include 'db_connect.php';

    $x = $_POST['x'];
    $y = $_POST['y'];
    $val = $_POST['val'];
    $time = date("Y-m-d H:i:s");

    $map_id ="map_".getMap();
    echo $map_id;


    // connect to the database
    $conn = Database::connect($map_id);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    $sql = "INSERT INTO radar_values(x, y, val, read_time)
            VALUES ($x, $y, $val, '$time')";
    $conn->exec($sql);

    Database::disconnect();

?>