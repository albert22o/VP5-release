#ifndef TABLEPARAMS_H
#define TABLEPARAMS_H

class TableParams
{
public:
    explicit TableParams(int columns, int rows, int padding);

    int Columns;
    int Rows;
    int Padding;
};

#endif // TABLEPARAMS_H
