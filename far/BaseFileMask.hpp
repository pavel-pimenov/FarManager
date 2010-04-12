#ifndef __BaseFileMask_HPP
#define __BaseFileMask_HPP

/*
BaseFileMask.hpp

����������� �����, ������� ��� �������� ������ � �������.
*/

extern const int EXCLUDEMASKSEPARATOR;

class BaseFileMask
{
	public:
		BaseFileMask() {}
		virtual ~BaseFileMask() {}

	public:
		virtual BOOL Set(const char *Masks, DWORD Flags)=0;
		virtual BOOL Compare(const char *Name)=0;
		virtual BOOL IsEmpty(void) { return TRUE; }

	private:
		BaseFileMask& operator=(const BaseFileMask& rhs); /* ����� �� */
		BaseFileMask(const BaseFileMask& rhs); /* �������������� �� ��������� */

};

#endif // __BaseFileMask_HPP
