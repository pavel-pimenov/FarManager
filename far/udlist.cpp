/*
udlist.cpp

������ ����-����, �������������� ����� ������-�����������. ���� �����, �����
������� ������ �������� �����������, �� ���� ������� ������� ��������� �
�������. ���� ����� ����������� ������ ������ � ������ ���, �� ���������, ���
��� �� �����������, � ������� ������.

*/

/* Revision: 1.04 02.07.2001 $ */

/*
Modify:
  02.07.2001 IS
    + AddAsterisk
  12.06.2001 IS
    + ��� ���������� ��������� ������ ������������ �������������� � ���������
      �������. ���� ����� ��������� �������, �� ������� ������ ������ ����
      �������� � �������
    + �������������� ���������� ������. ������ ����� �������� GetCommaWord �
      �������� ��� �������������� UserDefinedList
    + CheckSeparators()
  12.06.2001 IS
    + ���� ����� ����������� ������ ������ � ������ ���,
      �� ���������, ��� ��� �� �����������, � ������� ������
  09.06.2001 IS
    + ���������� � ������ ������� �����������. ������ ������������ ���. ��
      ��������� ��� ����� ';' � ','
  02.06.2001 IS
    + ������� � �����
*/

#include "headers.hpp"
#pragma hdrstop

#include "udlist.hpp"


UserDefinedList::UserDefinedList()
{
  DataCurrent=Data=DataEnd=NULL;
  SetSeparators(0,0,FALSE,FALSE);
}

UserDefinedList::UserDefinedList(BYTE separator1, BYTE separator2,
                                 BOOL processbrackets, BOOL addasterisk)
{
  DataCurrent=Data=DataEnd=NULL;
  SetSeparators(separator1, separator2, processbrackets, addasterisk);
}

void UserDefinedList::SetDefaultSeparators()
{
  Separator1=';';
  Separator2=',';
}

BOOL UserDefinedList::CheckSeparators() const
{
  return !((Separator1=='\"' || Separator2=='\"') ||
           (ProcessBrackets &&  (Separator1=='[' || Separator2=='[' ||
            Separator1==']' || Separator2==']'))
          );
}

BOOL UserDefinedList::SetSeparators(BYTE separator1, BYTE separator2,
                                    BOOL processbrackets, BOOL addasterisk)
{
  Free();
  Separator1=separator1;
  Separator2=separator2;
  ProcessBrackets=processbrackets;
  AddAsterisk=addasterisk;

  if(!Separator2 && !Separator2) SetDefaultSeparators();

  return CheckSeparators();
}

void UserDefinedList::Free()
{
  if(Data)
     free (Data);
  DataCurrent=Data=DataEnd=NULL;
}

BOOL UserDefinedList::Set(const char *List)
{
  Free();
  BOOL rc=FALSE;

  if(CheckSeparators() && List && *List)
  {
    if(*List!=Separator1 && *List!=Separator2)
      {
        int Length=0, Commas=0, RealLength;
        if(AddAsterisk)
          while(List[Length])
          {
            if(','==List[Length]) ++Commas; // �� ������� ����� �����������
                                            // ������ ��� ���������
            ++Length;
          }
        else
          Length=strlen(List);

        {
          Data=(char *)malloc(4+Length+Commas);
          DataEnd=Data;
          if(Data)
          {
            BOOL Error=FALSE;
            const char *CurList=List;
            while(NULL!=(CurList=Skip(CurList, Length, RealLength, Error)))
            {
              if(Length)
              {
                strncpy(DataEnd, CurList, Length);
                CurList+=RealLength;
                DataEnd[Length]=0;
                if(AddAsterisk && strpbrk(DataEnd,"?*.")==NULL)
                {
                  DataEnd[Length]='*';
                  ++Length;
                  DataEnd[Length]=0;
                }
                DataEnd+=Length+1;
              }
              else
              {
                Error=TRUE;
                break;
              }
            }

            rc=!Error;
          }
        }
      }
    else
      {
         const char *End=List+1;
         while(isspace(*End)) ++End; // ��������� �����
         if(!*End) // ���� ����� ����������� ������ ������ � ������ ���,
                   // �� ���������, ��� ��� �� �����������, � ������� ������
         {
           Data=(char *)malloc(4);
           if(Data)
           {
             DataEnd=Data+2;
             *Data=*List;
             Data[1]=0;
             rc=TRUE;
           }
         }
      }
  }

  if(rc)
      Start();
  else
      Free();

  return rc;
}

const char *UserDefinedList::Skip(const char *Str, int &Length, int &RealLength, BOOL &Error)
{
   Length=RealLength=0;
   Error=FALSE;

   while(isspace(*Str)) ++Str;
   if(*Str==Separator1 || *Str==Separator2) ++Str;
   while(isspace(*Str)) ++Str;
   if(!*Str) return NULL;

   const char *cur=Str;
   BOOL InBrackets=FALSE, InQoutes = (*cur=='\"');


   if(!InQoutes) // ���� �� � ��������, �� ��������� ����� ����� � ���� �������
     while(*cur) // �����! �������� *cur!=0 ������ ������ ������
     {
        if(ProcessBrackets)
        {
           if(*cur==']')
             InBrackets=FALSE;

           if(*cur=='[' && NULL!=strchr(cur+1, ']'))
             InBrackets=TRUE;
        }

        if(!InBrackets && (*cur==Separator1 || *cur==Separator2))
          break;

        ++cur;
     }

   if(!InQoutes || !*cur)
    {
      RealLength=Length=cur-Str;
      --cur;
      while(isspace(*cur))
       {
         --Length;
         --cur;
       }
      return Str;
    }

   // �� � �������� - �������� ��� ������ � �� ��������� �������
   ++cur;
   const char *QuoteEnd=strchr(cur, '\"');
   if(QuoteEnd==NULL)
    {
      Error=TRUE;
      return NULL;
    }

   const char *End=QuoteEnd+1;
   while(isspace(*End)) ++End;
   if(!*End || *End==Separator1 || *End==Separator2)
   {
     Length=QuoteEnd-cur;
     RealLength=End-cur;
     return cur;
   }

   Error=TRUE;
   return NULL;
}

void UserDefinedList::Start(void)
{
  DataCurrent=Data;
}

BOOL UserDefinedList::IsEmpty()
{
  return DataCurrent==NULL;
}

const char *UserDefinedList::GetNext()
{
  const char *ret=DataCurrent;
  if(DataCurrent)
  {
    DataCurrent+=strlen(DataCurrent)+1;
    if(DataCurrent>=DataEnd)
       DataCurrent=NULL;
  }

  return ret;
}
