set(my_SRC  "main.c" 
            "../BSP/src/BSP_WIFI.c" 
            "../BSP/src/BSP_ADC.c"
            "../BSP/src/BSP_MCPWM.c"
            "../BSP/src/BSP_GPIO.c"
            "../BSP/src/BSP_SPIADC.c"
            "../BSP/src/BSP_TIM.C"
            "../BSP/src/BSP_COMM.C"
            "../BSP/src/BSP_GPS.c"
            "../BSP/src/BSP_TVG.c"
            "../BSP/src/BSP_FLASH.c"
           )#源文件索引
            
idf_component_register(SRCS "${my_SRC}" #源文件列表
                    INCLUDE_DIRS "."    #头文件目录
                                 "../BSP/inc")
