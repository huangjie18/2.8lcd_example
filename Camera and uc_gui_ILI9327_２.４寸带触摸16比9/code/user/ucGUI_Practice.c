#include "ucGUI_Practice.h"
void ucGUI_Practice(void)
{
	int tx,ty;
	unsigned char i,page_num=1;

	Page_Init(page_num);
// 	while(1)
// 	{
// 		tx=TOUCH_Y();
// 		ty=TOUCH_X();
// 		GUI_DispDecAt( tx ,150,25,4);
// 		GUI_DispDecAt( ty ,200,25,4);			
// 		ty=GUI_TOUCH_X_MeasureY();
// 		tx=GUI_TOUCH_X_MeasureX();
// 		GUI_DispDecAt( tx ,150,5,4);
// 		GUI_DispDecAt( ty ,200,5,4);
// 	}

	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)==0)
		{
			tx=TOUCH_X();
			ty=TOUCH_Y();
			if(tx>250&&ty>200&&ty<240)return;
			i=ty/30;
			if(page_num==1)
			{
				TIM_Cmd(TIM2, ENABLE);
				switch (i)
				{
					case 0:
						MainTask_Dialog_All();
						Page_Init(page_num);
						break;
					case 1:
						DIALOG_SliderColor();
						Page_Init(page_num);
						break;
					case 2:
						WM_LateClipping();
						Page_Init(page_num);
						break;
					case 3:
						DIALOG_NestedModal();
						Page_Init(page_num);
						break;
					case 4:
						MainTask_WIDGET_Effect();
						Page_Init(page_num);
						break;
					case 5:
						MainTask_WIDGET_NumPad();
						Page_Init(page_num);
						break;
					case 6:
						MainTask_WIDGET_Multipage();
						Page_Init(page_num);
						break;
					case 7:
						MainTask_WIDGET_ButtonRound();
						Page_Init(page_num);
						break;
					default: break;	
				}
				TIM_Cmd(TIM2, DISABLE);
			}
			
		}
	}
}
void Page_Init(unsigned char page_num)
{
	GUI_RECT Rect = {340, 10, 390, 230};
	char acText[] = "Welcome to our shop\ntiky.taobao.com";
	switch (page_num)
	{
		case 1:
			Lcd_ColorBox(0,0,400,240,OLIVE);
			LCD_COLORINDEX=YELLOW;
			LCD_BKCOLORINDEX=BLUE;
			GUI_SetFont(&GUI_Font24_ASCII);
			GUI_DispStringAt("Dialog_All",0,30*0);
			GUI_DispStringAt("SliderColor",0,30*1);
			GUI_DispStringAt("LateClipping",0,30*2);
			GUI_DispStringAt("NestedModal",0,30*3);
			GUI_DispStringAt("WIDGET_Effect",0,30*4);
			GUI_DispStringAt("WIDGET_NumPad",0,30*5);
			GUI_DispStringAt("WIDGET_Multipage",0,30*6);
			GUI_DispStringAt("WIDGET_ButtonRound",0,30*7);
			GUI_DispStringAt("Back",264,216);
			break;
		default: break;
	}
	GUI_SetFont(&GUI_Font24_ASCII);
			GUI_FillRectEx(&Rect);
			GUI_SetColor(GUI_RED);
			GUI_SetBkColor(GUI_YELLOW);
			GUI_DispStringInRectEx(acText, 
                       &Rect, 
                       GUI_TA_HCENTER | GUI_TA_VCENTER, 
                       strlen(acText), 
                       GUI_ROTATE_CW);
}
