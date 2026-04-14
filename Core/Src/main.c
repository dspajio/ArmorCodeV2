/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

uint16_t MIC,tmp;  		//定义16位无符号整型变量MIC和tmp存储麦克风ADC采样值
uint16_t stat = 0;		//定义16位无符号整型变量stat存储PWM变化
uint8_t CHG;					//定义8位无符号整型变量CHG存储充电状态
uint8_t color;				//定义8位无符号整型变量color存储LED颜色
uint8_t light = 100;	//定义8位无符号整型变量light用于存储pwm占空比
uint8_t add = 0;			//定义8位无符号整型变量add用于存储加减状态


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void RED_Light();		
void GREEN_Light();
void BLUE_Light();
void Light_OFF(); 
void RGB_Light();
void Charge_Status();		//声明各子函数

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	RED_Light();				//开机默认亮红灯
	
  while (1)
  {			
		RGB_Light();			//调用RGB子函数
		Charge_Status();	//调用充电状态子函数
		HAL_TIM_PWM_Start((TIM_HandleTypeDef *) &htim3, (uint32_t) TIM_CHANNEL_1);		//使能定时器3 CH1
		HAL_TIM_PWM_Start((TIM_HandleTypeDef *) &htim3, (uint32_t) TIM_CHANNEL_2);		//使能定时器3 CH2
		HAL_TIM_PWM_Start((TIM_HandleTypeDef *) &htim3, (uint32_t) TIM_CHANNEL_3);		//使能定时器3 CH3 用于RGB三通道的MOS管PWM控制
		
		
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	//定时器中断服务函数
{
	if(htim->Instance == TIM1)					//若为定时器1
	{
		HAL_ADC_Start(&hadc1);						//ADC1启动转换
		MIC = HAL_ADC_GetValue(&hadc1);		//将ADC1的值赋给MIC
		HAL_ADC_Stop(&hadc1);							//ADC1停止转换
		if (MIC > tmp)	tmp = MIC;				//记录MIC的最大值
		
		if (MIC >= 3850)			//若MIC的ADC值大于3850则判断受到打击
		{
			switch (color)			//判断当前颜色
			{
				case 1:						//若为1红色
				{
					Light_OFF();
					HAL_Delay(40);
					RED_Light();
					HAL_Delay(40);
					Light_OFF();
					HAL_Delay(40);
					RED_Light();		//红色闪烁两次
					break;
				}
				case 2:						//若为2绿色
				{
					Light_OFF();
					HAL_Delay(40);
					GREEN_Light();
					HAL_Delay(40);
					Light_OFF();
					HAL_Delay(40);
					GREEN_Light();	//绿色闪烁两次
					break;
				}
				case 3:						//若为3蓝色
				{
					Light_OFF();
					HAL_Delay(40);
					BLUE_Light();
					HAL_Delay(40);
					Light_OFF();
					HAL_Delay(40);
					BLUE_Light();		//蓝色闪烁两次
					break;
				}
				
			}
		}
	}
}



void RED_Light() 			//红灯子函数
{
	color = 1;		 			//color赋1表示红色
	TIM3->CCR3 = 0;			//红色LED的MOS管PWM 100%
	TIM3->CCR2 = 100;		//绿色LED的MOS管PWM 0%
	TIM3->CCR1 = 1000;	//蓝色LED的MOS管PWM 0%
}	



void GREEN_Light() 		//绿灯子函数
{
	color = 2;					//color赋2表示绿色
	TIM3->CCR3 = 100;		//红色LED的MOS管PWM 0%
	TIM3->CCR2 = 0;			//绿色LED的MOS管PWM 100%
	TIM3->CCR1 = 100;		//蓝色LED的MOS管PWM 0%
}



void BLUE_Light()			//蓝灯子函数
{
	color = 3;					//color赋3表示蓝色
	TIM3->CCR3 = 100;		//红色LED的MOS管PWM 0%
	TIM3->CCR2 = 100;		//绿色LED的MOS管PWM 0%
	TIM3->CCR1 = 0;			//蓝色LED的MOS管PWM 100%
}



void Light_OFF()			//关灯子函数
{
	color = 0;       		//color赋0表示熄灭
	TIM3->CCR3 = 100;		//红色LED的MOS管PWM 0%
	TIM3->CCR2 = 100;		//绿色LED的MOS管PWM 0%
	TIM3->CCR1 = 100;		//蓝色LED的MOS管PWM 0%
}



void Charge_Status()	//充电呼吸灯子函数
{
	CHG = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);		//读取充电状态
		
	if (CHG == 1)				//若CHG为1则判断正在充电并启动绿色呼吸灯
	{
		color = 1;				//color赋1表示红色
		HAL_TIM_Base_Stop_IT((TIM_HandleTypeDef *) &htim1);	//禁用定时器1
		
		if(light == 105) 			//若light等于100
			{add = 0;}					//则add设为0减模式
		else if(light == 0) 	//若light等于0
			{add = 1;} 					//则add设为1加模式
			
		if(add == 0) 					//若add等于0减模式
			{light--;}					//则light自减
		else if(add == 1) 		//若add等于1加模式
			{light++;}					//则light自加
			
		TIM3->CCR3 = 100;			//红色LED的MOS管PWM 0%
		TIM3->CCR1 = 100;			//蓝色LED的MOS管PWM 0%
		TIM3->CCR2 = light;		//绿色LED的MOS管PWM为light
		HAL_Delay(15);				//延时10ms
	}
		
	else									//否则判断未在充电并恢复原状态
	{
		HAL_TIM_Base_Start_IT((TIM_HandleTypeDef *) &htim1);	//使能定时器1
		
		switch (color)			//判断当前状态
		{
			case 1:						//若为状态为1红色
			{
				RED_Light();		//则恢复红色状态
				break;
			}
			case 2:						//若状态为2绿色
			{
				GREEN_Light();	//则恢复绿色状态
				break;
			}
			case 3:						//若状态为3蓝色
			{
				BLUE_Light();		//则恢复蓝色状态
				break;
			}
		}
	}
}



void RGB_Light()		//RGB渐变子函数
{
	if (color == 2)		//若状态为2绿色
	{
		HAL_TIM_Base_Stop_IT((TIM_HandleTypeDef *) &htim1);	//禁用定时器1
	
		if (stat >= 50 && stat < 150)		//绿色到红色 light从100-0
		{	
			if(light == 100) 			//若light等于100
				{add = 0;}					//则add设为0减模式
			else if(light == 0) 	//若light等于0
				{add = 1;} 					//则add设为1加模式
							
			if(add == 0) 					//若add等于0减模式
				{light--;}					//则light自减
			else if(add == 1) 		//若add等于1加模式
				{light++;}					//则light自加
				
			TIM3->CCR1 = 100;					//蓝色LED的MOS管PWM100-light
			TIM3->CCR2 = 100 - light;	//绿色LED的MOS管PWM为light
			TIM3->CCR3 = light;				//红色LED的MOS管PWM为0
				
			HAL_Delay(12);						//延时12ms
		}
		
		else if (stat>=200 && stat < 300)	//红色到蓝色 light从0-100
		{
			if(light == 100) 			//若light等于100
				{add = 0;}					//则add设为0减模式
			else if(light == 0) 	//若light等于0
				{add = 1;} 					//则add设为1加模式
							
			if(add == 0) 					//若add等于0减模式
				{light--;}					//则light自减
			else if(add == 1) 		//若add等于1加模式
				{light++;}					//则light自加
			
			TIM3->CCR1 = 100 - light;	//蓝色LED的MOS管PWM100-light
			TIM3->CCR2 = 100;					//绿色LED的MOS管PWM为light
			TIM3->CCR3 = light;				//红色LED的MOS管PWM为0
				
			HAL_Delay(12);						//延时12ms
		}
		
		else if (stat>=350 && stat < 450)	//蓝色到绿色 light从100-0
		{
			if(light == 100) 			//若light等于100
				{add = 0;}					//则add设为0减模式
			else if(light == 0) 	//若light等于0
				{add = 1;} 					//则add设为1加模式
							
			if(add == 0) 					//若add等于0减模式
				{light--;}					//则light自减
			else if(add == 1) 		//若add等于1加模式
				{light++;}					//则light自加
					
			TIM3->CCR1 = 100 - light;	//蓝色LED的MOS管PWM100-light
			TIM3->CCR2 = light;				//绿色LED的MOS管PWM为light
			TIM3->CCR3 = 100;					//红色LED的MOS管PWM为0
				
			HAL_Delay(12);						//延时12ms
		}
		
		if (stat >= 450)	//完成一个循环后
		{
			light = 100;		//light复位为100
			stat = 0;				//stat复位为0
		}
		else							//未完成一个循环时
		{
			stat++;					//stat自加
		}					
	}
}
	



void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)	//外部中断回调函数
{
	if(GPIO_Pin == GPIO_PIN_1)	//若为外部中断1(红灯按钮)
		{
			RED_Light();						//调用红灯子函数
		}						
	if(GPIO_Pin == GPIO_PIN_2)	//若为外部中断2(蓝灯按钮)
		{
			BLUE_Light();						//调用蓝灯子函数
		}						
	if(GPIO_Pin == GPIO_PIN_3)	//若为外部中断3(绿灯按钮)
		{
			GREEN_Light();					//调用绿灯子函数(绿灯定义为RGB模式)
		}					
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
