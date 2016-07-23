/* uOS interrupt handlers for AT90USB647.
 * This file was generated by script 'mkintr.py' from data 'interrupt.cfg'.
 * Do not edit it manually! */

	HANDLE (0, clearb_r30 (0, EIMSK))
	HANDLE (1, clearb_r30 (1, EIMSK))
	HANDLE (2, clearb_r30 (2, EIMSK))
	HANDLE (3, clearb_r30 (3, EIMSK))
	HANDLE (4, clearb_r30 (4, EIMSK))
	HANDLE (5, clearb_r30 (5, EIMSK))
	HANDLE (6, clearb_r30 (6, EIMSK))
	HANDLE (7, clearb_r30 (7, EIMSK))
	HANDLE (8, clearb_r30 (PCIE0, PCICR))
	HANDLE (11, clearb_r30 (WDIE, WDTCSR))
	HANDLE (12, clearb_r30 (OCIE2A, TIMSK2))
	HANDLE (13, clearb_r30 (OCIE2B, TIMSK2))
	HANDLE (14, clearb_r30 (TOIE2, TIMSK2))
	HANDLE (15, clearb_r30 (ICIE1, TIMSK1))
	HANDLE (16, clearb_r30 (OCIE1A, TIMSK1))
	HANDLE (17, clearb_r30 (OCIE1B, TIMSK1))
	HANDLE (18, clearb_r30 (OCIE1C, TIMSK1))
	HANDLE (19, clearb_r30 (TOIE1, TIMSK1))
	HANDLE (20, clearb_r30 (OCIE0A, TIMSK0))
	HANDLE (21, clearb_r30 (OCIE0B, TIMSK0))
	HANDLE (22, clearb_r30 (TOIE0, TIMSK0))
	HANDLE (23, clearb_r30 (SPIE, SPCR))
	HANDLE (24, clearb_r30 (RXCIE1, UCSR1B))
	HANDLE (25, clearb_r30 (UDRIE1, UCSR1B))
	HANDLE (26, clearb_r30 (TXCIE1, UCSR1B))
	HANDLE (27, clearb_r30 (ACIE, ACSR))
	HANDLE (28, clearb_r30 (ADIE, ADCSRA))
	HANDLE (29, clearb_r30 (3, EECR))
	HANDLE (30, clearb_r30 (ICIE3, TIMSK3))
	HANDLE (31, clearb_r30 (OCIE3A, TIMSK3))
	HANDLE (32, clearb_r30 (OCIE3B, TIMSK3))
	HANDLE (33, clearb_r30 (OCIE3C, TIMSK3))
	HANDLE (34, clearb_r30 (TOIE3, TIMSK3))
	HANDLE (35, clearb_r30 (TWIE, TWCR))
	HANDLE (36, clearb_r30 (SPMIE, SPMCSR))

void arch_intr_allow (int irq)
{
	switch ((unsigned char) irq) {
	case 0: setb (0, EIMSK); break;
	case 1: setb (1, EIMSK); break;
	case 2: setb (2, EIMSK); break;
	case 3: setb (3, EIMSK); break;
	case 4: setb (4, EIMSK); break;
	case 5: setb (5, EIMSK); break;
	case 6: setb (6, EIMSK); break;
	case 7: setb (7, EIMSK); break;
	case 8: setb (PCIE0, PCICR); break;
	case 11: setb (WDIE, WDTCSR); break;
	case 12: setb (OCIE2A, TIMSK2); break;
	case 13: setb (OCIE2B, TIMSK2); break;
	case 14: setb (TOIE2, TIMSK2); break;
	case 15: setb (ICIE1, TIMSK1); break;
	case 16: setb (OCIE1A, TIMSK1); break;
	case 17: setb (OCIE1B, TIMSK1); break;
	case 18: setb (OCIE1C, TIMSK1); break;
	case 19: setb (TOIE1, TIMSK1); break;
	case 20: setb (OCIE0A, TIMSK0); break;
	case 21: setb (OCIE0B, TIMSK0); break;
	case 22: setb (TOIE0, TIMSK0); break;
	case 23: setb (SPIE, SPCR); break;
	case 24: setb (RXCIE1, UCSR1B); break;
	case 25: break;
	case 26: setb (TXCIE1, UCSR1B); break;
	case 27: setb (ACIE, ACSR); break;
	case 28: setb (ADIE, ADCSRA); break;
	case 29: setb (3, EECR); break;
	case 30: setb (ICIE3, TIMSK3); break;
	case 31: setb (OCIE3A, TIMSK3); break;
	case 32: setb (OCIE3B, TIMSK3); break;
	case 33: setb (OCIE3C, TIMSK3); break;
	case 34: setb (TOIE3, TIMSK3); break;
	case 35: setb (TWIE, TWCR); break;
	case 36: setb (SPMIE, SPMCSR); break;
	}
}
