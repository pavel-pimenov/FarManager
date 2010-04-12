/*
xlat.cpp

XLat - �������������

*/

#include "headers.hpp"
#pragma hdrstop

#include "plugin.hpp"
#include "global.hpp"
#include "fn.hpp"

// $ 05.09.2000 SVS -  XLat-�������������!
char* WINAPI Xlat(
    char *Line,                    // �������� ������
    int StartPos,                  // ������ �������������������
    int EndPos,                    // ����� �������������������
    const struct CharTableSet *TableSet, // ���������������� ������� (����� ���� NULL)
    DWORD Flags)                   // ����� (��. enum XLATMODE)
{
	BYTE Chr,ChrOld;
	int J,I;
	int PreLang=2,CurLang=2; // uncnown
	int LangCount[2]={0,0};
	int IsChange=0;

	/* $ 08.09.2000 SVS
	   �������� �������� :-)))
	*/
	if (!Line || *Line == 0)
		return NULL;

	/* SVS $ */
	I=(int)strlen(Line);

	if (EndPos > I)
		EndPos=I;

	if (StartPos < 0)
		StartPos=0;

	if (StartPos > EndPos || StartPos >= I)
		return Line;

//  FAR_OemToCharBuff(Opt.QWERTY.Table[0],Opt.QWERTY.Table[0],80);???
	if (!Opt.XLat.Table[0][0] || !Opt.XLat.Table[1][0])
		return Line;

	int MinLenTable=(BYTE)Opt.XLat.Table[1][0];

	if ((BYTE)Opt.XLat.Table[1][0] > (BYTE)Opt.XLat.Table[0][0])
		MinLenTable=(BYTE)Opt.XLat.Table[0][0];

	if (TableSet)
		// �� ������� ��������� � OEM
		DecodeString(Line+StartPos,(LPBYTE)TableSet->DecodeTable,EndPos-StartPos+1);

	char LayoutName[64];
	int ProcessLayoutName=FALSE;

	if ((Flags & XLAT_USEKEYBLAYOUTNAME) && FARGetKeybLayoutName(LayoutName,sizeof(LayoutName)-1))
	{
		GetRegKey("XLat",LayoutName,(BYTE*)&Opt.XLat.Rules[2][1],(BYTE*)"",sizeof(Opt.XLat.Rules[2]));

		if (Opt.XLat.Rules[2][1])
			ProcessLayoutName=TRUE;
	}

	// ���� �� ���� ������
	for (J=StartPos; J < EndPos; J++)
	{
		ChrOld=Chr=(BYTE)Line[J];
		// ChrOld - ���� ������
		IsChange=0;

		// ���� �� ��������� Chr � ��������
		// <=MinLenTable ��� ��� ����� ��������� � ��������� ������ 1
		for (I=1; I <= MinLenTable; ++I)
		{
			// ������ �� ��������?
			if (Chr == (BYTE)Opt.XLat.Table[1][I])
			{
				Chr=(char)Opt.XLat.Table[0][I];
				IsChange=1;
				CurLang=1; // pred - english
				LangCount[1]++;
				break;
			}
			// ������ �� �������?
			else if (Chr == (BYTE)Opt.XLat.Table[0][I])
			{
				Chr=(char)Opt.XLat.Table[1][I];
				CurLang=0; // pred - russian
				LangCount[0]++;
				IsChange=1;
				break;
			}
		}

		if (!IsChange) // ������ ������...
		{
			if (ProcessLayoutName)
			{
				for (I=1; I < Opt.XLat.Rules[2][0]; I+=2)
					if (Chr == (BYTE)Opt.XLat.Rules[2][I])
					{
						Chr=(BYTE)Opt.XLat.Rules[2][I+1];
						break;
					}
			}
			else
			{
				PreLang=CurLang;

				if (LangCount[0] > LangCount[1])
					CurLang=0;
				else if (LangCount[0] < LangCount[1])
					CurLang=1;
				else
					CurLang=2;

				if (PreLang != CurLang)
					CurLang=PreLang;

				for (I=1; I < Opt.XLat.Rules[CurLang][0]; I+=2)
					if (ChrOld == (BYTE)Opt.XLat.Rules[CurLang][I])
					{
						Chr=(BYTE)Opt.XLat.Rules[CurLang][I+1];
						break;
					}

#if 0

				// ���� � ������� �� ������� � ������� ���� Unknown...
				if (I >= Opt.XLat.Rules[CurLang][0] && CurLang == 2)
				{
					// ...������� ������� � ������ �������...
					for (I=1; I < Opt.XLat.Rules[0][0]; I+=2)
						if (ChrOld == (BYTE)Opt.XLat.Rules[0][I])
							break;

					for (J=1; J < Opt.XLat.Rules[1][0]; J+=2)
						if (ChrOld == (BYTE)Opt.XLat.Rules[1][J])
							break;

					if (I >= Opt.XLat.Rules[0][0])
						CurLang=1;

					if (J >= Opt.XLat.Rules[1][0])
						CurLang=0;

					if ()//???
					{
						Chr=(BYTE)Opt.XLat.Rules[CurLang][J+1];
					}
				}

#endif
			}
		}

		Line[J]=(char)Chr;
	}

	if (TableSet)
		// �� OEM � ������� ���������
		EncodeString(Line+StartPos,(LPBYTE)TableSet->EncodeTable,EndPos-StartPos+1);

	// ����������� ��������� ����������?
	//  � ��������� �� �������� ��� Win9x - ������� WinNT � ������������� :-)
	if ((Flags & XLAT_SWITCHKEYBLAYOUT))
	{
		if (!hFarWnd)
			InitDetectWindowedMode();

		if (hFarWnd)
		{
			HKL Next=(HKL)0;

			if (Opt.XLat.Layouts[0])
			{
				if (++Opt.XLat.CurrentLayout >= (int)sizeof(Opt.XLat.Layouts)/sizeof(Opt.XLat.Layouts[0]) || !Opt.XLat.Layouts[Opt.XLat.CurrentLayout])
					Opt.XLat.CurrentLayout=0;

				if (Opt.XLat.Layouts[Opt.XLat.CurrentLayout])
					Next=Opt.XLat.Layouts[Opt.XLat.CurrentLayout];
			}

			PostMessage(hFarWnd,WM_INPUTLANGCHANGEREQUEST, Next?0:INPUTLANGCHANGE_FORWARD, (LPARAM)Next);

			if (Flags & XLAT_SWITCHKEYBBEEP)
				MessageBeep(0);
		}
	}

	return Line;
}
