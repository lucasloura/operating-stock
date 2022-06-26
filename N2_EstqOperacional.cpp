//	Adryellen das Graças Santana
//	Lucas Oliveira Loura

#include <stdio.h>
#include <stdlib.h>

struct TProducts
{
	int prodctCode, startQty, minimQty;
};

struct TSales
{
	int prodctCode, soldQty, salesSit, salesChl;
};

struct TTransfer
{
	int prodctCode, OCQty, minimQty, soldQty, necQty, stckAftrSales, transfStrg2OC;
};

struct TDivergencies
{
	int line, reason;
};

int doesProdctExists(int code, TTransfer *vS, int sizS);

int main (void)
{
	TProducts product;
	TSales *sales, auxSales;
	TTransfer *transfer, auxTransfer;
	TDivergencies *divergs;
	FILE *datFile;
	int sizSales = 0, sizTransfer = 0, sizDivergs = 0;
	int totRepres = 0, totWebsite = 0, totAndroid = 0, totIPhone = 0;
	int pos, i, iAux;


	// checking the validity of the codes, as well as already reading and storing info about the products
	// and allocating them to the transfer array


	datFile = fopen("produtos.txt", "r");

	transfer = NULL;

	while ( fscanf(datFile, "%d;%d;%d", & product.prodctCode, & product.startQty, & product.minimQty) != EOF )
	{
		if (product.prodctCode >= 10000 && product.prodctCode <= 99999)
		{
			sizTransfer++;
			transfer = (TTransfer *)realloc(transfer, sizeof(TTransfer) * sizTransfer);
			transfer[sizTransfer - 1].prodctCode = product.prodctCode;
			transfer[sizTransfer - 1].OCQty = product.startQty;
			transfer[sizTransfer - 1].minimQty = product.minimQty;
			transfer[sizTransfer - 1].soldQty = 0;
			transfer[sizTransfer - 1].necQty = 0;			
		}
	}

	fclose(datFile);


	// reading and storing info about the sales


	datFile = fopen("vendas.txt", "r");

	sales = NULL;

	while ( fscanf(datFile, "%d;%d;%d;%d", & auxSales.prodctCode, & auxSales.soldQty, & auxSales.salesSit, & auxSales.salesChl ) != EOF )
	{
		sizSales++;
		sales = (TSales *)realloc(sales, sizeof(TSales) * sizSales);
		sales[sizSales - 1] = auxSales;
	}

	fclose(datFile);


	// summing the sold quantities product by product, channel by channel as well as signaling if a
	// product's code is valid and also if it's in fact a confirmed sale


	divergs = NULL;

	for (i = 0; i < sizSales; i++)
	{
		pos = doesProdctExists(sales[i].prodctCode, transfer, sizTransfer);

		if (pos >= 0)	// if the product's code is valid
		{
			if (sales[i].salesSit == 100 || sales[i].salesSit == 102)	// if it's a confirmed sale
			{
				transfer[pos].soldQty += sales[i].soldQty;

				if (sales[i].salesChl == 1)
					totRepres += sales[i].soldQty;
				else if (sales[i].salesChl == 2)
					totWebsite += sales[i].soldQty;
				else if (sales[i].salesChl == 3)
					totAndroid += sales[i].soldQty;
				else
					totIPhone += sales[i].soldQty;
			}
			else	// if it's not a confirmed sale
			{
				sizDivergs++;
				divergs = (TDivergencies *)realloc(divergs, sizeof(TDivergencies) * sizDivergs);

				if (sales[i].salesSit == 135)	// if it's a cancelled sale
				{
					divergs[sizDivergs - 1].line = i + 1;
					divergs[sizDivergs - 1].reason = 1;
				}
				else if (sales[i].salesSit == 190)	// if it's an unfinished sale
				{
					divergs[sizDivergs - 1].line = i + 1;
					divergs[sizDivergs - 1].reason = 2;
				}
				else	// if it's an unknown error
				{
					divergs[sizDivergs - 1].line = i + 1;
					divergs[sizDivergs - 1].reason = 3;
				}
			}
		}
		else	// if the product's code isn't valid
		{
			sizDivergs++;
			divergs = (TDivergencies *)realloc(divergs, sizeof(TDivergencies) * sizDivergs);
			divergs[sizDivergs - 1].line = i + 1;

			if (sales[i].salesSit == 999)
				divergs[sizDivergs - 1].reason = 3;
			else
				divergs[sizDivergs - 1].reason = sales[i].prodctCode;
		}
	}


	// obtaining the stock after sales' values, the necessary reposition quantity as well the quantity
	// to be transfered from the storage to the operational center


	for (i = 0; i < sizTransfer; i++)
	{
		transfer[i].stckAftrSales = transfer[i].OCQty - transfer[i].soldQty;
		
		for (iAux = transfer[i].stckAftrSales; iAux < transfer[i].minimQty; iAux++)
			transfer[i].necQty++;
		
		if (transfer[i].necQty >= 1 && transfer[i].necQty <= 10)
			transfer[i].transfStrg2OC = 10;
		else
			transfer[i].transfStrg2OC = transfer[i].necQty;
	}


	// printing and writing all of the obtained transfer values


	printf("Necessidade de Transferencia Armazem para CO\n\n");
	printf("Produto  QtCO  QtMin  QtVendas  Estq.apos  Necess.  Transf. de\n");
	printf("                                   Vendas            Arm p/ CO\n");
	for (i = 0; i < sizTransfer; i++)
		printf("%5d   %5d  %5d     %5d      %5d    %5d       %5d\n", transfer[i].prodctCode, transfer[i].OCQty, transfer[i].minimQty, transfer[i].soldQty, transfer[i].stckAftrSales, transfer[i].necQty, transfer[i].transfStrg2OC);

	datFile = fopen("transfere.txt", "w");

	fprintf(datFile, "Necessidade de Transferência Armazém para CO\n\n");
	fprintf(datFile, "Produto  QtCO  QtMin  QtVendas  Estq.apos  Necess.  Transf. de\n");
	fprintf(datFile, "                                   Vendas            Arm p/ CO\n");
	for (i = 0; i < sizTransfer; i++)
		fprintf(datFile, "%5d   %5d  %5d     %5d      %5d    %5d       %5d\n", transfer[i].prodctCode, transfer[i].OCQty, transfer[i].minimQty, transfer[i].soldQty, transfer[i].stckAftrSales, transfer[i].necQty, transfer[i].transfStrg2OC);

	fclose(datFile);


	// printing and writing all of the detected divergencies


	printf("\n");

	for (i = 0; i < sizDivergs; i++)
	{
		printf("Linha %d - ", divergs[i].line);

		 if (divergs[i].reason == 1)
			printf("Venda cancelada\n");
		else if (divergs[i].reason == 2)
			printf("Venda nao finalizada\n");
		else if (divergs[i].reason == 3)
			printf("Erro desconhecido. Acionar equipe de TI.\n");
		else
			printf("Codigo de Produto nao encontrado %5d\n", divergs[i].reason);
	}

	datFile = fopen("divergencias.txt", "w");

	for (i = 0; i < sizDivergs; i++)
	{
		fprintf(datFile, "Linha %d - ", divergs[i].line);

		 if (divergs[i].reason == 1)
			fprintf(datFile, "Venda cancelada\n");
		else if (divergs[i].reason == 2)
			fprintf(datFile, "Venda não finalizada\n");
		else if (divergs[i].reason == 3)
			fprintf(datFile, "Erro desconhecido. Acionar equipe de TI.\n");
		else
			fprintf(datFile, "Código de Produto não encontrado %5d\n", divergs[i].reason);
	}

	fclose(datFile);


	// printing and writing the total sales sold by each channel


	printf("\n");

	printf("Quantidades de Vendas por canal\n\n");
	printf("Canal                  QtVendas\n");
	printf("1 - Representantes       %6d\n", totRepres);
	printf("2 - Website              %6d\n", totWebsite);
	printf("3 - App movel Android    %6d\n", totAndroid);
	printf("4 - App movel iPhone     %6d\n", totIPhone);

	datFile = fopen("totcanal.txt", "w");

	fprintf(datFile, "Quantidades de Vendas por canal\n\n");
	fprintf(datFile, "Canal                  QtVendas\n");
	fprintf(datFile, "1 - Representantes       %6d\n", totRepres);
	fprintf(datFile, "2 - Website              %6d\n", totWebsite);
	fprintf(datFile, "3 - App movel Android    %6d\n", totAndroid);
	fprintf(datFile, "4 - App movel iPhone     %6d\n", totIPhone);

	fclose(datFile);

	return (0);
}

int doesProdctExists(int code, TTransfer *pS, int sizS)
{
	int i;

	for (i = 0; i < sizS; i++)
	{
		if (code == pS[i].prodctCode)
			return i;		
	}
	return (-1);
}
