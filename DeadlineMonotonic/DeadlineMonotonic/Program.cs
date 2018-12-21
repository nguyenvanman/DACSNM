using System;
using System.Collections.Generic;
using System.Linq;

namespace DeadlineMonotonic
{
    class Program
    {
        private static List<ReadyTask> readyTasks;

        private static List<Task> Tasks = new List<Task>();

        static void Main(string[] args)
        {
            InputTasks();


            Console.WriteLine();
            Console.WriteLine("\tWCET\tDeadline\tPeriod");
            foreach (var task in Tasks)
            {
                Console.WriteLine($"{task.Name}\t{task.Capacity}\t{task.Deadline}\t\t{task.Period}");
            }

            Console.WriteLine();
            Console.WriteLine($"CPU Utilization: U = {CalculateUltilization()}");

            Console.WriteLine();
            Console.WriteLine("Earlier Deadline First");
            Console.WriteLine();
            EarlierDeadlineFirstScheduling();
            Console.ReadKey();
        }

        private static void DeadlineMonotonicScheduling()
        {
            Scheduling(InitReadyTaskForDeadLineMonotonic, false);
        }

        private static void RateMonotonicScheduling()
        {
            Scheduling(InitReadyTaskForRateMonotonic, false);
        }

        private static void EarlierDeadlineFirstScheduling()
        {
            Scheduling(InitReadyTaskForEarlierDeadlineFirst, true);
        }

        private static void Scheduling(Action initAction, bool isEdf)
        {
            initAction.Invoke();
            for (var i = 1; i <= CalculateHyperPeriod(); i++)
            {
                Console.Write("Time " + i + ": ");
                var index = 0;
                var isProcessing = false;
                while (index < readyTasks.Count)
                {
                    if (readyTasks[index].CurrentCapacity > 0)
                    {
                        Console.WriteLine(readyTasks[index].Task.Name);
                        readyTasks[index].CurrentCapacity--;
                        isProcessing = true;
                        if (isEdf) UpdateCurrentDeadline(i);
                        break;
                    }
                    index++;
                }
                if (!isProcessing) Console.WriteLine();

                foreach (var t in readyTasks)
                {
                    if (i % t.Task.Period == 0)
                        t.CurrentCapacity = t.Task.Capacity;
                }
            }
        }

        private static void InputTasks()
        {
            Console.Write("Enter the number of tasks: ");
            var numberOfTasks = Convert.ToInt32(Console.ReadLine());

            for (var i = 0; i < numberOfTasks; i++)
            {
                Console.WriteLine($"Enter the information of task {i + 1}:");
                Console.Write("Capacity: ");
                var capacity = Convert.ToInt32(Console.ReadLine());
                Console.Write("Deadline: ");
                var deadline = Convert.ToInt32(Console.ReadLine());
                Console.Write("Period: ");
                var period = Convert.ToInt32(Console.ReadLine());

                Tasks.Add(new Task
                {
                    Name = $"Task {i + 1}",
                    Capacity = capacity,
                    Deadline = deadline,
                    Period = period
                });
            }
        }

        private static void InitReadyTaskForDeadLineMonotonic()
        {
            readyTasks = Tasks
                .OrderBy(x => x.Deadline)
                .Where(x => x.Deadline != 0)
                .Select(x => new ReadyTask
                {
                    Task = x,
                    CurrentCapacity = x.Capacity
                })
            .ToList();
        }

        private static void InitReadyTaskForRateMonotonic()
        {
            readyTasks = Tasks
                .OrderBy(x => x.Period)
                .Select(x => new ReadyTask
                {
                    Task = x,
                    CurrentCapacity = x.Capacity
                })
            .ToList();
        }

        private static void InitReadyTaskForEarlierDeadlineFirst()
        {
            readyTasks = Tasks.OrderBy(x => x.Deadline)
                .Where(x => x.Deadline != 0)
                .Select(x => new ReadyTask
                {
                    Task = x,
                    CurrentCapacity = x.Capacity,
                    CurrentDeadline = x.Deadline
                })
                .ToList();
        }

        private static void UpdateCurrentDeadline(int time)
        {
            readyTasks.ForEach(x => x.CurrentDeadline = (time / x.Task.Deadline) * x.Task.Period + x.Task.Deadline);
            readyTasks = readyTasks.OrderBy(x => x.CurrentDeadline).ToList();
        }

        private static float CalculateUltilization()
        {
            var u = 0f;
            foreach (var task in Tasks)
            {
                u += task.Capacity * 1f / task.Period;
            }

            return u;
        }

        private static int Gcd(int x, int y)
        {
            while (y != 0)
            {
                int tmp = x % y;
                x = y;
                y = tmp;
            }

            return x;
        }

        private static int Lcm(int x, int y)
        {
            return x * y / Gcd(x, y);
        }

        private static int CalculateHyperPeriod()
        {
            var hyper = 1;
            foreach (var task in Tasks)
            {
                hyper = Lcm(hyper, task.Period);
            }
            return hyper;
        }
    }
}








