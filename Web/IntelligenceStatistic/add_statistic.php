<?php
date_default_timezone_set('Asia/Shanghai');

$begin_time = $_POST["_begin_time"];
$time_consumed = $_POST["_time_consumed"];
$lock_enemy = $_POST["_lock_enemy"];
$lock_winner = $_POST["_lock_winner"];
$winner = $_POST["_winner"];
$log_id = $_POST["_log_id"];
$income = $_POST["_income"];
$client_ver = $_POST["_client_ver"];
$client_machine_code = $_POST["_client_machine_code"];
$client_ip = getIp();

$begin_time = date('Y-m-d H:i:s',$begin_time);


$con = mysqli_connect("localhost","root","","intelligence_player");
if (!$con)
{
    echo "err";
    exit();
}

//mysql_select_db("intelligence_player",$con);
$request_str = "INSERT INTO _statistic_totally (
    _begin_time, _time_consumed, 
    _lock_enemy, _lock_winner, _winner, _log_id,
    _income, _client_ver, _client_machine_code, 
    _client_ip) 
 VALUES ('$begin_time', '$time_consumed', 
'$lock_enemy','$lock_winner','$winner',
'$log_id','$income','$client_ver','$client_machine_code',
'$client_ip');";
if (!mysqli_query($con,$request_str))
{
    echo "err2:".mysqli_error($con);
    exit();
}
echo "1";

function getIp()
{

    if(!empty($_SERVER["HTTP_CLIENT_IP"]))
    {
        $cip = $_SERVER["HTTP_CLIENT_IP"];
    }
    else if(!empty($_SERVER["HTTP_X_FORWARDED_FOR"]))
    {
        $cip = $_SERVER["HTTP_X_FORWARDED_FOR"];
    }
    else if(!empty($_SERVER["REMOTE_ADDR"]))
    {
        $cip = $_SERVER["REMOTE_ADDR"];
    }
    else
    {
        $cip = '';
    }
    preg_match("/[\d\.]{7,15}/", $cip, $cips);
    $cip = isset($cips[0]) ? $cips[0] : 'unknown';
    unset($cips);

    return $cip;
}

?>