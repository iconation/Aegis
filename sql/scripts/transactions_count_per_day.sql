SELECT 
    DATE(FROM_UNIXTIME(block.timestamp DIV 1000000)),
    COUNT(DISTINCT id) totalCount
FROM
    iconation.block
GROUP BY DATE(FROM_UNIXTIME(block.timestamp DIV 1000000))