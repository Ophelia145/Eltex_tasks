#include "main.h"

#define rand_quantity_item_shop (rand () % 10000 + 1)
#define rand_quantity_needs_client (rand () % 100000 + 1)
volatile int supplier_is_working = 1;
// pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *
client_routine (void *arg)
{
  market_t *market = (market_t *)arg;
  int needs_after, items_after;

  while (market->client.needs > 0)
    {
      for (int i = 0; i < SHOP_COUNT; i++)
        {
          if (pthread_mutex_trylock (&market->shops[i].mutex) == 0)
            {
              if (market->shops[i].items > 0)
                {

                  if (market->client.needs >= market->shops[i].items)
                    {
                      items_after = 0;
                      needs_after
                          = market->client.needs - market->shops[i].items;
                    }
                  else if (market->client.needs < market->shops[i].items)
                    {
                      needs_after = 0;
                      items_after
                          = market->shops[i].items - market->client.needs;
                    }

                  printf ("\nПокупатель %d, потребность - %d, в магазине %d "
                          "было %d "
                          "товаров, купил %d товаров, потребность стала %d\n",
                          market->client.index, market->client.needs,
                          market->shops[i].index, market->shops[i].items,
                          market->client.needs - needs_after, needs_after);

                  market->shops[i].items = items_after;
                  market->client.needs = needs_after;
                  pthread_mutex_unlock (&market->shops[i].mutex);
                  sleep (2);
                }
              else
                {
                  pthread_mutex_unlock (&market->shops[i].mutex);
                }
              if (market->client.needs == 0)
                {
                  printf ("\nПокупатель %d удовлетворил свои потребности\n",
                          market->client.index);
                  break;
                }
            }
        }
    }
  printf ("\nПокупатель %d  завершился\n", market->client.index);
  pthread_exit (NULL);
  return NULL; //НАДО ?
}

void *
supplier (void *arg)
{
  shop_t *shops = (shop_t *)arg; // А ДИНАМИЧЕСКУЮ ТУТ НОРМ ВООБЩЕ??????
  while (supplier_is_working)
    {
      for (int i = 0; i < SHOP_COUNT; i++)
        {
          if (pthread_mutex_trylock (&shops[i].mutex) == 0)
            {
              if (shops[i].items == 0)
                {

                  shops[i].items = 5000;
                  printf ("\nПогрузчик снабдил магазин %d товаром в размере "
                          "%d штук\n",
                          shops[i].index, 5000);
                }
              pthread_mutex_unlock (&shops[i].mutex);
              sleep (1);
            }
        }
    }
  return NULL;
}

void
market ()
{
  shop_t shared_shops[SHOP_COUNT];
  for (int i = 0; i < SHOP_COUNT; i++)
    {

      shared_shops[i].index = i;
      shared_shops[i].items = rand_quantity_item_shop;
      if (pthread_mutex_init (&shared_shops[i].mutex, NULL) != 0)
        {
          perror ("mutex init failed");
          return;
        }

      printf ("\n\nВ магазине %d столько товаров: %d\n\n\n",
              shared_shops[i].index, shared_shops[i].items);
    }
  pthread_t client_thread[CLIENT_COUNT];
  market_t markets[CLIENT_COUNT];
  for (int i = 0; i < CLIENT_COUNT; i++)
    {

      markets[i].shops = shared_shops;

      markets[i].client.index = i;
      markets[i].client.needs = rand_quantity_needs_client;
      printf ("\n\n Покупатель %d хочет: %d ед.\n\n\n",
              markets[i].client.index, markets[i].client.needs);

      if (pthread_create (&client_thread[i], NULL, client_routine,
                          (void *)&markets[i])
          != 0)
        {
          perror ("Failed to create client thread");
          exit (EXIT_FAILURE);
        }
    }

  pthread_t supplier_thread;
  if (pthread_create (&supplier_thread, NULL, supplier, (void *)shared_shops)
      != 0)
    {
      perror ("Failed to create sup thread");
      exit (EXIT_FAILURE);
    }
  else
    {
      printf ("\nsupplier did its work\n");
    }
  for (int i = 0; i < CLIENT_COUNT; i++)
    {
      pthread_join (client_thread[i], NULL);
    }

  supplier_is_working = 0;
  pthread_join (supplier_thread, NULL);
}
