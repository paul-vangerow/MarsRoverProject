<?php

// used to get data from the db into json string notation for get requests 
function print_json($qr, $col){
    echo "[";
    foreach($qr as $row){
        echo "{";
        $comma = false;
        foreach($col as $c){
            if(!$comma){
                $comma=true;
            }
            else{
                echo ",";
            }
            echo '"'.$c.'"'.':';
            if(is_string($row[$c])){
                echo '"'.$row[$c].'"';
            }
            else{
                echo $row[$c];
            }
        }
        echo "}";
    }
    echo "]"; 
}


?>