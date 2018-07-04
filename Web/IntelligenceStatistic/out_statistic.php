<?php

if ($_GET["Ac"]!="intelligence_player")
{
  exit();
}

$mysqli = new MySQLi("localhost","root","","intelligence_player");
if(!$mysqli){
  die($mysqli->error);
}

function showTable($mysqli,$table_name){ 
  $sql = "select * from $table_name";
  $res = $mysqli->query($sql);
 //获取返回总行数和列数
 //echo "共有：".$res->num_rows."行；共有：".$res->field_count."列。";
 //获取表头---从$res取出
  echo "<table border=1><tr>";
  while($field=$res->fetch_field()){
    echo "<th>{$field->name}</th>";
  }
  echo "</tr>";
  //循环取出数据
  while($row=$res->fetch_row())
  {
    echo "<tr>";
    foreach($row as $value)
    {
      echo "<td>$value</td>";
    }
    echo "</tr>";
  }
  echo "</table>"; 
  $res->free();
}

showTable($mysqli,"_statistic_totally");
$mysqli->close();

?>