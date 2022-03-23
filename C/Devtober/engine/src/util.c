#include <util.h>
#include <time.h>

void le_print_time() {
    time_t current_time;
    struct tm * m_time; 

    time(&current_time); 
    m_time = localtime(&current_time); 

    printf("[%d/%d/%d -> %d:%d]", m_time -> tm_mday, m_time -> tm_mon, m_time -> tm_year + 1900, m_time -> tm_hour, m_time -> tm_min);

}
